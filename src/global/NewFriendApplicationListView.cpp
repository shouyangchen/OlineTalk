#include "NewFriendApplicationListView.h"
#include "NewFriendApplicationListDelegate.h"
#include "NewFriendApplicationListModel.h"
#include <QMouseEvent>
#include <QMenu>
#include <QJsonObject>

#include "TcpMgr.h"
#include "user_info_mgr.h"

NewFriendApplicationListView::NewFriendApplicationListView(QWidget* parent):QListView(parent)
{
	this->setItemDelegate(new NewFriendApplicationListDelegate(this));
	QListView::setModel(new NewFriendApplicationListModel());
	this->setSelectionRectVisible(false);//选中的背影不可见
	this->setEditTriggers(EditTrigger::NoEditTriggers);//不可编辑
	//安装事件过滤器到视图的视口，这样可以捕获到所有的鼠标事件
	this->viewport()->installEventFilter(this);
	accept_action = new QAction(u8"同意", this);
	reject_action = new QAction(u8"拒绝", this);
	connect(accept_action, &QAction::triggered, this, &NewFriendApplicationListView::acceptApplication);
	connect(reject_action, &QAction::triggered, this, &NewFriendApplicationListView::rejectApplication);
}


bool NewFriendApplicationListView::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == this->viewport())
	{
		if (event->type() == QEvent::MouseButtonPress)
		{
			auto mouse_event = static_cast<QMouseEvent*>(event);
			if (mouse_event->button() == Qt::RightButton)
			{
				auto index = this->indexAt(mouse_event->pos());
				auto mouse_pos = mouse_event->pos();
				//将鼠标当前的位置转换为全局位置
				auto global_pos = this->viewport()->mapToGlobal(mouse_pos);
				if (index.isValid())
				{
					this->setCurrentIndex(index);//设置当前选中项
					qDebug() << "Right-clicked on valid index at row:" << index.row();
					auto menu = new QMenu(this);
					menu->addAction(accept_action);
					menu->addAction(reject_action);
					menu->exec(global_pos);
				}
			}
		}
	}
	return QListView::eventFilter(watched, event);
}



void NewFriendApplicationListView::acceptApplication()
{
	auto index = this->currentIndex();
	if (!index.isValid())
		return;
	//获取当前选中的申请信息
	auto model = dynamic_cast<NewFriendApplicationListModel*>(this->model());
	auto application = model->data(index, NewFriendApplicationListModel::UserIdRole).toString();
	int status;
	switch (status= model->data(index, NewFriendApplicationListModel::StatusRole).toInt())
	{
	case 1://已同意
		return;
	case 2://已拒绝
		return;
	default:
		{
		QJsonObject obj;
		obj["application_user_id"] = static_cast<double>(application.toDouble());
		obj["target_user_id"] = static_cast<double>(user_info_mgr::getInstance(QPixmap{}, QString{}, QString{}, std::uint64_t{})->get_user_id());//当前用户id
		obj["status"] = "accept";
		model->setData(index, 1, NewFriendApplicationListModel::FriendApplicationRoles::StatusRole);//更新状态为已同意
		QJsonDocument doc(obj);
		TcpMgr::getInstance()->send_data(Message_id::ADD_FRIEND_RESPONSE, doc.toJson());
		//显示一个界面让用户设置备注名和分组（如果需要的话）
		//TODO:实现备注名和分组设置界面

		//通过信号通知最近联系人列表和好友列表更新
		auto application_user = model->get_application_user_info(index);
		the_connected_user_info new_friend_info(application_user.user_id, application_user.username, application_user.avatar);
		connectUserList::user_info connect_user_info(application_user.user_id, application_user.username, application_user.avatar);
		emit sig_accept_application_no_recent_chat_list(new_friend_info);//通过信号通知最近联系人列表和好友列表更新
		emit sig_accept_application_on_connect_user_list(connect_user_info);//通过信号通知联系人列表更新
		}
	}

	
}



void NewFriendApplicationListView::rejectApplication()
{
	auto index = this->currentIndex();
	if (!index.isValid())
		return;
	//获取当前选中的申请信息
	auto model = dynamic_cast<NewFriendApplicationListModel*>(this->model());
	auto application = model->data(index, NewFriendApplicationListModel::UserIdRole).toString();
	int status;
	switch (status = model->data(index, NewFriendApplicationListModel::StatusRole).toInt())
	{
	case 1://已同意
		return;
	case 2://已拒绝
		return;
	default:
	{
		QJsonObject obj;
		obj["application_user_id"] = application.toDouble();
		obj["target_user_id"] = static_cast<double>(user_info_mgr::getInstance(QPixmap{}, QString{}, QString{}, std::uint64_t{})->get_user_id());//当前用户id
		obj["status"] = "reject";
		model->setData(index, 2, NewFriendApplicationListModel::FriendApplicationRoles::StatusRole);//更新状态为已拒绝
		QJsonDocument doc(obj);
		TcpMgr::getInstance()->send_data(Message_id::ADD_FRIEND_RESPONSE, doc.toJson());
	}
	}

}

NewFriendApplicationListView::~NewFriendApplicationListView()
{
	delete this->model();
}