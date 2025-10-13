#include "NewFriendApplicationListDelegate.h"
#include <QPainter>
#include <QPainterPath>
#include "NewFriendApplicationListModel.h"
NewFriendApplicationListDelegate::NewFriendApplicationListDelegate(QObject* parent)
	:QStyledItemDelegate(parent)
{
}

void NewFriendApplicationListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	//需要绘制用户的头像以及验证消息内容以时间和状态
	//图标在左侧，申请消息在中间，时间在右侧然后状态在最右侧
	//绘制背景
	painter->save();
	painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);//启用反锯齿和文本反锯齿
	//绘制背景
	QRect rect = option.rect;
	QPoint topLeft = rect.topLeft();
	QRect icon_rect = QRect(topLeft.x() + 10, topLeft.y() + 30, 50, 50);
	QRect user_name_rect = QRect(icon_rect.right() + 20, topLeft.y() + 10, 150, 35);
	QRect message_rect = QRect(icon_rect.right() + 20, topLeft.y() +60, rect.width()-icon_rect.width()-150, 20);
	QRect time_rect = QRect(message_rect.right() + 5, topLeft.y() + 10, 150, 20);
	QRect status_rect = QRect(message_rect.right() + 10, topLeft.y() + 60, 80, 20);
	//绘制图标
	QPixmap icon = index.data(NewFriendApplicationListModel::AvatarRole).value<QPixmap>();
	qDebug() << "Icon size:" << icon.size();
	if (!icon.isNull())
	{
		QPainterPath path;
		path.addEllipse(icon_rect);
		painter->setClipPath(path);
		painter->drawPixmap(icon_rect, icon);
		painter->setClipping(false);
	}
	//绘制用户名
	QString user_name = index.data(NewFriendApplicationListModel::UserNameRole).toString();
	QFont font = painter->font();
	QFont old_font = font;
	font.setPointSize(15);//设置字体大小
	font.setBold(true);
	painter->setFont(font);
	painter->setPen(Qt::black);
	painter->drawText(user_name_rect, Qt::AlignLeft | Qt::AlignVCenter, user_name);
	painter->setFont(old_font);
	
	//绘制消息
	QString message = index.data(NewFriendApplicationListModel::MsgRole).toString();
	painter->setPen(Qt::black);
	painter->drawText(message_rect, Qt::AlignLeft | Qt::AlignVCenter, QString(u8"验证消息:")+message);
	//绘制时间
	QDateTime time = index.data(NewFriendApplicationListModel::TimeRole).toDateTime();
	painter->setPen(Qt::gray);
	painter->drawText(time_rect, Qt::AlignLeft | Qt::AlignVCenter, time.toString("yyyy-MM-dd hh:mm"));
	//绘制状态
	int status = index.data(NewFriendApplicationListModel::StatusRole).toInt();
	//status 0:未处理 1:已同意 2:已拒绝
	if (status)
	{
		QBrush brush = status == 1 ? QBrush(Qt::white) : QBrush(Qt::red);
		painter->setBrush(brush);
		painter->setPen(Qt::NoPen);
		painter->drawRoundedRect(status_rect.adjusted(10, 0, -10, 0), 10, 10);
		painter->setPen(Qt::white);
		QString status_text = status == 1 ? "已同意" : "已拒绝";
		painter->drawText(status_rect, Qt::AlignCenter, status_text);
	}
	else
	{
		QBrush brush = QBrush(QColor(18,233,141));
		painter->setBrush(brush);
		painter->setPen(Qt::NoPen);
		painter->drawRoundedRect(status_rect.adjusted(10, 0, -10, 0), 10, 10);
		painter->setPen(Qt::white);
		painter->drawText(status_rect, Qt::AlignCenter, "未处理");
	}
	painter->restore();
	QStyledItemDelegate::paint(painter, option, index);
}

QSize NewFriendApplicationListDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	return {option.rect.width(), 80};
}