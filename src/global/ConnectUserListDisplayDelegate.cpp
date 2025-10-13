#include "ConnectUserListDisplayDelegate.h"

#include "ConnectUserListModel.h"

ConnectUserListDisplayDelegate::ConnectUserListDisplayDelegate(QObject* parent) :QStyledItemDelegate(parent)
{
	
}

QSize ConnectUserListDisplayDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	return QSize(option.rect.width(), 60);//每个item的高度为60
}

void ConnectUserListDisplayDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	painter->save();
	//绘制背景
	if (option.state & QStyle::State_Selected)
	{
		painter->fillRect(option.rect, option.palette.highlight());
	}
	else
	{
		painter->fillRect(option.rect, option.palette.base());
	}
	//绘制头像
	if (option.state & QStyle::State_Selected) {
		auto brush = option.palette.highlight();
		brush.setColor(QColor(245, 245, 245));
		painter->fillRect(option.rect, brush);
	}
	else {
		painter->fillRect(option.rect, option.palette.base());
	}
	QVariant avatarData = index.data(ConnectUserListModel::connectListModelRole::UserAvatarRole);
	QPixmap avatarPixmap;
	if (avatarData.canConvert<QPixmap>())
	{
		avatarPixmap = avatarData.value<QPixmap>();
	}
	else
	{
		avatarPixmap = QPixmap(":/default_avatar.png"); // 使用默认头像
	}
	QRect avatarRect = option.rect.adjusted(5, 5, -option.rect.width() + 55, -5);
	painter->drawPixmap(avatarRect, avatarPixmap.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	//绘制用户名
	QString username = index.data(ConnectUserListModel::connectListModelRole::UserNameRole).toString();
	QRect textRect = option.rect.adjusted(65, 0, -5, 0);
	painter->setPen(option.palette.text().color());
	painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, username);
	painter->restore();
}