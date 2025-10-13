#include "RecentChatUsersListDelegate.h"
#include <QPainterPath>
QSize RecentChatUsersListDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	return QSize(option.rect.width(), 70); // 固定高度为 70 像素
}

void RecentChatUsersListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);// 启用抗锯齿等渲染提示
	painter->save();
	// 获取数据
	if (!index.isValid())
		return;
	QString user_name = index.data(RecentChatListModel::UserNameRole).toString();
	QPixmap user_avatar = index.data(RecentChatListModel::UserAvatarRole).value<QPixmap>();
	QString last_message = index.data(RecentChatListModel::LastMessageRole).toString();
	QDateTime last_message_time = index.data(RecentChatListModel::LastMessageTimeRole).toDateTime();
	int unread_message_count = index.data(RecentChatListModel::UnreadMessageCountRole).toInt();
	// 绘制背景
	if (option.state & QStyle::State_Selected) {
		auto brush = option.palette.highlight();
		brush.setColor(QColor(245, 245, 245));
		painter->fillRect(option.rect,brush);
	} else {
		painter->fillRect(option.rect, option.palette.base());
	}
	// 绘制头像
	int avatar_size = 50;
	QRect avatar_rect(option.rect.left() + 10, option.rect.top() + (option.rect.height() - avatar_size) / 2, avatar_size, avatar_size);//在左侧垂直居中
	if (!user_avatar.isNull()) {
		QPixmap scaled_avatar = user_avatar.scaled(avatar_size, avatar_size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
		QPainterPath path;
		path.addEllipse(avatar_rect);
		painter->setClipPath(path);
		painter->drawPixmap(avatar_rect, scaled_avatar);
		painter->setClipping(false);// 取消裁剪
	} else {
		// 绘制默认头像
		painter->setBrush(Qt::lightGray);
		painter->setPen(Qt::NoPen);
		painter->drawEllipse(avatar_rect);
	}
	// 绘制用户名
	int text_x = avatar_rect.right() + 10;
	int text_width = option.rect.width() - text_x - 10;
	int text_height = (option.rect.height() - 20) / 2;
	QRect user_name_rect(text_x, option.rect.top() + 10, text_width, text_height);
	painter->setPen(option.palette.text().color());
	painter->setFont(QFont("Arial", 10, QFont::Bold));
	painter->drawText(user_name_rect, Qt::AlignLeft | Qt::AlignVCenter, user_name);
	// 绘制最后一条消息
	QRect last_message_rect(text_x, user_name_rect.bottom(), text_width, text_height);
	painter->setFont(QFont("Arial", 9));
	painter->setPen(option.palette.text().color().darker());
	painter->drawText(last_message_rect, Qt::AlignLeft | Qt::AlignVCenter, last_message);
	// 绘制时间
	if (last_message_time.isValid()) {
		QString time_text;
		QDateTime now = QDateTime::currentDateTime();
		if (last_message_time.date() == now.date()) {
			time_text = last_message_time.toString("HH:mm");
		} else if (last_message_time.date() == now.addDays(-1).date()) {
			time_text = "昨天 " + last_message_time.toString("HH:mm");
		} else {
			time_text = last_message_time.toString("MM-dd HH:mm");
		}
		QRect time_rect(option.rect.right() - 100, option.rect.top() + 10, 90, 20);
		painter->setFont(QFont("Arial", 8));
		painter->setPen(option.palette.text().color().darker());
		painter->drawText(time_rect, Qt::AlignRight | Qt::AlignVCenter, time_text);
	}
	// 绘制未读消息数
	if (unread_message_count > 0) {
		QString unread_text = QString::number(unread_message_count);
		QFont font("Arial", 8, QFont::Bold);
		QFontMetrics fm(font);
		int badge_width = qMax(20, fm.horizontalAdvance(unread_text) + 10);
		int badge_height = 20;
		QRect badge_rect(option.rect.right() - badge_width - 10, option.rect.bottom() - badge_height - 10, badge_width, badge_height);
		painter->setBrush(Qt::red);
		painter->setPen(Qt::NoPen);
		painter->drawEllipse(badge_rect);
		painter->setFont(font);
		painter->setPen(Qt::white);
		painter->drawText(badge_rect, Qt::AlignCenter, unread_text);
	}
	painter->restore();

}
