#include "NewFriendApplicationListModel.h"
#include <QDebug>

#include "HistoryDB_Mgr.h"
#include "the_user_icon_mgr.h"

NewFriendApplicationListModel::NewFriendApplicationListModel(QObject* parent) :QAbstractListModel(parent)
{
	connect(HistoryDB_Mgr::getInstance().get(), &HistoryDB_Mgr::sig_applicationListReady, this, &NewFriendApplicationListModel::slot_history_applications_loading_done);
	connect(the_user_icon_mgr::getInstance().get(), &the_user_icon_mgr::sig_applications_user_icon_loading_done, this, &NewFriendApplicationListModel::slot_the_application_list_user_avatar_loading_done);
	this->getApplicationList();//获取申请列表为了避免在构造函数中直接调用异步函数导致的问题，延时1秒再获取
	this->getApplicationListAvatar();//获取申请列表头像
}


int NewFriendApplicationListModel::rowCount(const QModelIndex& parent) const
{
	return static_cast<int>(this->model.size());
}

QVariant NewFriendApplicationListModel::data(const QModelIndex& index, int role) const
{
	auto row = index.row();
	if (row < 0 || row >= this->model.size()) {
		return {};
	}
	const auto& item = this->model.at(row);
	switch (role) {
	case AvatarRole:
		return item.avatar;
	case UserNameRole:
		return item.username;
	case MsgRole:
		return item.msg;
	case TimeRole:
		return  item.time;
	case StatusRole:
		return item.status;
	default:
		return {};
	}
}

void NewFriendApplicationListModel::slot_history_applications_loading_done(const QList<NewFriendApplicationlNS::user_application>& application_list)
{
	qDebug() << "slot_history_applications_loading_done called with" << application_list.size() << "applications";
	for (int i=0;i<application_list.size();++i)
	{
		this->model.append(application_list.at(i));
		emit dataChanged(this->index(i, 0), this->index(i, 0));//通知视图数据更新
	}
}


void NewFriendApplicationListModel::slot_the_application_list_user_avatar_loading_done(const QList<std::pair<QPixmap, QString>>& user_avatar_list)
{
	//更新申请列表中的头像
	for (const auto& avatar_pair : user_avatar_list)
	{
		for (int i = 0; i < this->model.size(); ++i)
		{
			if (this->model.at(i).user_id == avatar_pair.second)
			{
				this->model[i].avatar = avatar_pair.first;
				qDebug() << "Updated avatar for user_id:" << avatar_pair.second;
				emit dataChanged(this->index(i, 0), this->index(i, 0), { AvatarRole });//只通知头像数据更新
				break;
			}
		}
	}
}


void NewFriendApplicationListModel::storeApplicationList()const
{
	HistoryDB_Mgr::getInstance()->slot_storeApplicationList(this->model);//存储好友申请列表
}


void NewFriendApplicationListModel::slot_new_application_insert_model(const NewFriendApplicationlNS::user_application& application)
{
	QAbstractListModel::beginInsertRows(QModelIndex(), 0, 0);//在最前面插入
	this->model.insert(0, application);
	QAbstractListModel::endInsertRows();

}


NewFriendApplicationListModel::~NewFriendApplicationListModel()
{
	this->storeApplicationList();
	qDebug() << "~NewFriendApplicationListModel() called";
}

void NewFriendApplicationListModel::getApplicationList()
{
	emit HistoryDB_Mgr::getInstance()->sig_requestApplicationsList();//请求异步加载好友申请列表
}

void NewFriendApplicationListModel::getApplicationListAvatar()
{
	if (this->model.isEmpty())
	{
		QTimer::singleShot(1000, this, &NewFriendApplicationListModel::getApplicationListAvatar);//延时1秒再获取，避免在构造函数中直接调用异步函数导致的问题
	}
	else
	{
		QList<QString>user_id_list;
		for (const auto& app : this->model)
		{
			user_id_list.append(app.user_id);
		}
		the_user_icon_mgr::getInstance()->get_applications_user_icon_async(user_id_list);//异步获取申请列表头像
		qDebug() << "Requested application list avatars for" << user_id_list.size() << "users";
	}
}
