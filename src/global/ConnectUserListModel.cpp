#include "ConnectUserListModel.h"
#include "HistoryDB_Mgr.h"
#include "the_user_icon_mgr.h"
#include <QDebug>
#include <QTimer>
#include <QFutureWatcher>

ConnectUserListModel::ConnectUserListModel(QObject* parent) : QAbstractListModel(parent)
{
    qDebug() << "Creating ConnectUserListModel";

    // 连接数据库管理器信号 - 接收联系人列表数据
    connect(HistoryDB_Mgr::getInstance().get(), &HistoryDB_Mgr::sig_connectUserListReady,
        this, &ConnectUserListModel::slot_update_connect_user_list);

    // 连接信号与槽函数，当需要加载用户头像或者更新用户头像时调用the_user_icon_mgr的异步获取用户头像函数
    connect(this, &ConnectUserListModel::sig_need_loading_user_icon_or_update_user_icon,
        the_user_icon_mgr::getInstance().get(), &the_user_icon_mgr::slot_get_user_icon_async);

    // 连接信号与槽函数，当异步获取用户头像完成后调用更新联系人列表头像函数
    connect(the_user_icon_mgr::getInstance().get(), &the_user_icon_mgr::sig_notify_the_user_icon_loading_done,
        this, &ConnectUserListModel::slot_update_connect_user_icon);
    // 初始化头像刷新定时器
    get_user_icon_timer = new QTimer(this);
    get_user_icon_timer->setInterval(120000); // 每两分钟获取一次用户头像
    get_user_icon_timer->setSingleShot(false);

    connect(get_user_icon_timer, &QTimer::timeout, [this]() {
        emit this->sig_need_loading_user_icon_or_update_user_icon(this->user_name_model); // 发射信号通知需要加载用户头像或者更新用户头像
    });

    get_user_icon_timer->start(); // 启动定时器

    // 请求联系人列表
    emit HistoryDB_Mgr::getInstance()->sig_request_connect_user_list();
    emit this->sig_need_loading_user_icon_or_update_user_icon(this->user_name_model);
    qDebug() << "ConnectUserListModel created successfully";
}

int ConnectUserListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(user_name_model.size());
}

QVariant ConnectUserListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= user_name_model.size()) {
        return {};
    }

    const auto& user_key = user_name_model.at(index.row());
    auto it = this->model.find(user_key);
    if (it == this->model.end()) {
        return {};
    }

    const auto& user = it.value();

    switch (role) {
    case connectListModelRole::UserNameRole:
        return user.username;
    case connectListModelRole::UserIdRole:
        return user.user_id;
    case connectListModelRole::UserAvatarRole:
        return user.avatar;
    default:
        return {};
    }
}

bool ConnectUserListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= user_name_model.size()) {
        return false;
    }

    const auto& user_key = user_name_model.at(index.row());
    auto it = this->model.find(user_key);
    if (it == this->model.end()) {
        return false;
    }

    auto& user = it.value();

    switch (role) {
    case connectListModelRole::UserNameRole:
        user.username = value.toString();
        emit dataChanged(index, index, { role });
        return true;
    case connectListModelRole::UserIdRole:
        user.user_id = value.toString();
        emit dataChanged(index, index, { role });
        return true;
    case connectListModelRole::UserAvatarRole:
        user.avatar = value.value<QPixmap>();
        emit dataChanged(index, index, { role });
        return true;
    default:
        return false;
    }
}

void ConnectUserListModel::slot_update_connect_user_list(const QList<connectUserList::user_info>& user_list)
{
    qDebug() << "ConnectUserListModel: Received user list with" << user_list.size() << "users";

    beginResetModel();
    this->model.clear();
    this->user_name_model.clear();

    for (const auto& user : user_list) {
        this->model.insert(user.user_id, user);
        this->user_name_model.append(user.user_id);
        qDebug() << "ConnectUserListModel: Added user:" << user.user_id << "name:" << user.username;
    }

    endResetModel();

    qDebug() << "ConnectUserListModel: Model updated, final size:" << model.size();
    
    // 发射信号通知用户列表已更新
    emit userListUpdated(user_list.size());
    
    emit sig_need_loading_user_icon_or_update_user_icon(this->user_name_model);
}

void ConnectUserListModel::slot_update_connect_user_icon(const QList<std::pair<QPixmap, QString>>& icon_list)
{
    for (const auto& icon_pair : icon_list) {
        const auto& user_id = icon_pair.second;
        auto it = this->model.find(user_id);
        if (it != this->model.end()) {
            auto& user = it.value();
            user.avatar = icon_pair.first;

            // 发射数据改变信号以更新视图
            int row = static_cast<int>(user_name_model.indexOf(user_id));
            if (row != -1) {
                QModelIndex idx = this->index(row);
                emit dataChanged(idx, idx, { connectListModelRole::UserAvatarRole });
            }
        }
    }
}

void ConnectUserListModel::get_user_icon()
{
    for (int i = 0; i < user_name_model.size(); i++) {
        const auto& user_key = user_name_model.at(i);
        auto it = this->model.find(user_key);
        if (it == this->model.end()) continue;
        auto& user = it.value();
        if (user.avatar.isNull()) {
            user.avatar = the_user_icon_mgr::getInstance()->get_user_icon(user.user_id, QString{}); // 获取用户头像

            // 发射数据改变信号以更新视图
            QModelIndex idx = this->index(i);
            emit dataChanged(idx, idx, { connectListModelRole::UserAvatarRole });
        }
    }
}

void ConnectUserListModel::refreshUserList()
{
    qDebug() << "ConnectUserListModel: Manual refresh requested";
    emit HistoryDB_Mgr::getInstance()->sig_request_connect_user_list();
}

void ConnectUserListModel::refreshUserIcons()
{
    qDebug() << "ConnectUserListModel: Manual icon refresh requested";
}


void ConnectUserListModel::slot_get_users_name(const QList<QString>& user_id_list)
{
	qDebug() << "doing get users name";
    QList<std::pair<QString, QString>>users_name;
	for (auto&item:user_id_list)
	{
		auto it = this->model.find(item);
        if (it == this->model.end())
			continue;
		auto& user = it.value();//获取用户信息
        if (!user.username.isEmpty())
        {
			users_name.append(std::make_pair(user.user_id, user.username));
		}
        
    }
	emit sig_connect_users_name_loading_done(users_name);//发射信号通知用户名称加载完成
	qDebug() << "id " << user_id_list.size() << "name " << users_name.size();
}


void ConnectUserListModel::slot_update_user_name(const std::pair<QString, QString>&user)
{
    auto it = this->model.find(user.first);
    if (it==this->model.end())
    {
	    return;
    }
	auto& u = it.value();
    u.username = user.second;
    int row = static_cast<int>(user_name_model.indexOf(user.first));
    if (row != -1) {
        QModelIndex idx = this->index(row);
        emit dataChanged(idx, idx, { connectListModelRole::UserNameRole });
	}
	emit sig_connect_user_name_changed(user); // 发射信号通知用户名称改变
}
