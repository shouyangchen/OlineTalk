#include "ConnectUserListView.h"
#include <QMouseEvent>

#include "ConnectUserListModel.h"
#include "global.h"

ConnectUserListView::ConnectUserListView(QWidget*parent)
	: QListView(parent)
{
	setSelectionMode(QAbstractItemView::SingleSelection);
	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->viewport()->installEventFilter(this);
}




bool ConnectUserListView::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == this->viewport() && event->type() == QEvent::MouseButtonPress) {
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
		QPoint pos = mouseEvent->pos();
		QModelIndex index = this->indexAt(pos);
		if (index.isValid()) {
			connectUserList::user_info info;
			info.user_id = index.data(ConnectUserListModel::UserIdRole).toString();
			info.username = index.data(ConnectUserListModel::UserNameRole).toString();
			info.avatar = index.data(ConnectUserListModel::UserAvatarRole).value<QPixmap>();
			//先获取当前选中的用户信息
			if (event->type() == QEvent::MouseButtonDblClick) {
				//TODO :展示用户信息的详细资料界面
			}
		}
	}
	return QListView::eventFilter(watched, event);
}

ConnectUserListView::~ConnectUserListView()
{
	auto model = this->model();
	delete model;
}