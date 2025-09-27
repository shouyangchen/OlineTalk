#include "ChatHistoryDisplayDelegate.h"
#include <QApplication>
#include <QTextOption>
#include <QStaticText>
#include <QStyleOptionViewItem>
#include "user_info_mgr.h"

ChatHistoryDisplayDelegate::ChatHistoryDisplayDelegate(QObject* parent):QStyledItemDelegate(parent)
{
    this->sender_user_icon = the_user_icon_mgr::getInstance()->get_user_icon(
        QString::number(user_info_mgr::getInstance(QPixmap{}, QString{}, QString{},
            std::uint64_t{})->get_user_id()), QString{});
    this->my_icon = the_user_icon_mgr::getInstance()->get_user_icon(
        QString::number(user_info_mgr::getInstance(QPixmap{}, QString{}, QString{},
            std::uint64_t{})->get_user_id()), QString{});
}

void ChatHistoryDisplayDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->save();
    auto  item_state = option.state;
    // 获取消息类型和发送者
    QString message_type = index.data(Qt::UserRole + 2).toString();
    QString sender = index.data(Qt::UserRole + 3).toString();

    int avatar_size = 40;
    int padding = 10;
    int bubble_padding = 8; // 气泡内部padding
    int max_width = 300;

    QRect rect = option.rect;
    QSize content_size = this->sizeHint(option, index);

    bool isSelf = (sender == "SELF");
    QPixmap avatar;
    QRect avatar_rect, bubble_rect;
    if (isSelf) {
        // 右侧头像
        avatar = this->my_icon;
        avatar_rect = QRect(rect.right() - avatar_size - padding, rect.top() + padding, avatar_size, avatar_size);
        bubble_rect = QRect(rect.right() - avatar_size - 2 * padding - content_size.width(),
            rect.top() + padding,
            content_size.width(),
            content_size.height());
    }
    else {
        // 左侧头像
        avatar = this->sender_user_icon;
        avatar_rect = QRect(rect.left() + padding, rect.top() + padding, avatar_size, avatar_size);
        bubble_rect = QRect(avatar_rect.right() + padding,
            rect.top() + padding,
            content_size.width(),
            content_size.height());
    }

    // 绘制头像
    if (!avatar.isNull()) {
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        painter->drawPixmap(avatar_rect, avatar.scaled(avatar_size, avatar_size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    else {
        painter->setBrush(Qt::gray);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(avatar_rect);
    }

    // 绘制气泡背景
    QBrush bubble_brush = isSelf ? QBrush(QColor(0, 220, 120)) : QBrush(Qt::white);
    painter->setBrush(bubble_brush);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(bubble_rect, 10, 10);

    // 消息内容区域（气泡内部padding）
    QRect content_rect = bubble_rect.adjusted(bubble_padding, bubble_padding, -bubble_padding, -bubble_padding);

    painter->setPen(Qt::black);

    if (message_type == "text") {
        QString text = index.data(Qt::DisplayRole).toString();
        QTextOption text_option;
        text_option.setWrapMode(QTextOption::WordWrap);
        painter->setFont(option.font);
        painter->drawText(content_rect, text, text_option);
    }
    else if (message_type == "image") {
        QByteArray image_data = QByteArray::fromBase64(index.data(Qt::DisplayRole).toByteArray());
        QPixmap pixmap;
        if (pixmap.loadFromData(image_data)) {
            painter->drawPixmap(content_rect, pixmap.scaled(content_rect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        else {
            painter->drawText(content_rect, "Invalid Image Data");
        }
    }
    else if (message_type == "video") {
        painter->fillRect(content_rect, Qt::black);
        painter->setPen(Qt::white);
        painter->drawText(content_rect, Qt::AlignCenter, "Video Message");
    }
    // ...其他类型
        //如果被选中那么就在最有侧边绘制一个小绿圈
    if (item_state.testFlag(QStyle::State_Selected))
    {
        // 如果被选中那么就在最右侧边绘制一个小绿圈
        int circle_radius = 5;
        int circle_diameter = circle_radius * 2;
        int circle_x = option.rect.right() + padding - circle_diameter - 3;
        int circle_y = option.rect.top() + padding * 2; // 你可以根据实际需求调整纵向位置

        QRect circle_rect(circle_x, circle_y, circle_diameter, circle_diameter);

        painter->setBrush(QColor(0, 220, 120)); // 与气泡颜色保持一致
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(circle_rect);
    }

    painter->restore();
}

QSize ChatHistoryDisplayDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	auto message_data = index.data(Qt::UserRole + 2).toString();//获取消息类型
	if (message_data == "text")
	{
		return this->calculate_text_widget_size(index.data(Qt::DisplayRole).toString());
	}
	else if (message_data == "image")
	{
		auto image_data =QByteArray::fromBase64(index.data(Qt::DisplayRole).toByteArray());
		return this->calculate_image_widget_size(image_data);

	}
	else if (message_data == "video")
	{
		return this->calculate_video_widget_size(QByteArray::fromBase64(index.data(Qt::DisplayRole).toByteArray()));

	}
    return  QStyledItemDelegate::sizeHint(option, index);
}


void ChatHistoryDisplayDelegate::slot_replace_the_user_icon(const QPixmap& icon)
{
		this->sender_user_icon = icon;
}

QSize ChatHistoryDisplayDelegate::calculate_text_widget_size(QString const& text) const
{
    QFont font = QApplication::font();
    int max_width = 300; // 最大气泡宽度
    int padding = 10;

    QFontMetrics fm(font);
    QRect rect = fm.boundingRect(QRect(0, 0, max_width, INT_MAX),
        Qt::TextWordWrap | Qt::AlignLeft,
        text);

    // 气泡宽度自适应内容，但不超过最大宽度
    int bubble_width = std::min(rect.width(), max_width) + 2 * padding;
    int bubble_height = rect.height() + 2 * padding;
    return QSize(bubble_width, bubble_height);
}

QSize ChatHistoryDisplayDelegate::calculate_image_widget_size(QByteArray const& image_data) const
{
	QPixmap pixmap;
	if (!pixmap.loadFromData(image_data))
	{
		return QSize(100, 100); // 返回一个默认大小
	}
	// 限制最大尺寸
	int max_dimension = 200; // 最大宽高
	int width = pixmap.width();
	int height = pixmap.height();
	if (width > max_dimension || height > max_dimension)
	{
		if (width > height)
		{
			height = static_cast<int>(height * (static_cast<double>(max_dimension) / width));
			width = max_dimension;
		}
		else
		{
			width = static_cast<int>(width * (static_cast<double>(max_dimension) / height));
			height = max_dimension;
		}
	}
	return QSize(width + 10, height + 10); // 加上内边距
}

QSize ChatHistoryDisplayDelegate::calculate_video_widget_size(QByteArray const& video_data) const
{

	return QSize(300, 200); // 固定大小
}

QSize ChatHistoryDisplayDelegate::calculate_file_widget_size(QString const& file_name) const
{

	return QSize(250, 50); // 固定大小
}

QSize ChatHistoryDisplayDelegate::calculate_audio_widget_size(QByteArray const& audio_data) const
{

	return QSize(250, 50); // 固定大小
}

void ChatHistoryDisplayDelegate::slot_replace_the_my_icon(const QPixmap& icon)
{
    this->my_icon = icon;
}
