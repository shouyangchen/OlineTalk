#include "RecentChatUsersListModel.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <algorithm>
#include <QThread>

#include "the_user_icon_mgr.h"
#include "HistoryDB_Mgr.h"

RecentChatListModel::RecentChatListModel(QObject* parent):QAbstractListModel(parent)
{
	model_cache = new QCache<QString, ChatHistoryDataModel>(100);//最多缓存100个model

	// 选择可写数据目录 (与 UI 线程无关, 只需 QCoreApplication 已初始化)
	data_dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	if (data_dir.isEmpty()) {
		data_dir = QDir::currentPath(); // 兜底
	}
	QDir dir(data_dir);
	if (!dir.exists()) {
		dir.mkpath(".");
	}
	QString file_path = dir.filePath("the_chat_weight.dat");

	// 读取热度表 (失败不提前 return, 保证后续定时器仍然工作)
	{
		QFile file(file_path);
		if (file.open(QIODevice::ReadOnly)) {
			QDataStream in(&file);
			in.setVersion(QDataStream::Qt_6_0);
			in >> the_chat_weight;
			if (the_chat_weight.isEmpty()) {
				the_chat_weight = QMap<QString, int>();
			}
		} else {
			qWarning() << u8"读取热度表失败:" << file.errorString() << " path=" << file_path;
		}
	}

	// 初始化触碰状态表(将已有权重的会话标为未触碰, 方便老化)
	for (auto it = the_chat_weight.constBegin(); it != the_chat_weight.constEnd(); ++it) {
		was_touched_chat.insert(it.key(), false);
	}

	// 设置定时器 (假设该 model 运行在有事件循环的线程)
	timer = new QTimer(this);
	constexpr int kCheckIntervalMs = 120000; // 2 分钟
	timer->setInterval(kCheckIntervalMs);
	connect(timer, &QTimer::timeout, this, &RecentChatListModel::check_the_last_touch_chat_and_weight);
	timer->start();
	connect(this, &RecentChatListModel::sig_the_recent_user_list_loading, HistoryDB_Mgr::getInstance().get(), &HistoryDB_Mgr::getRecentChatUserListAsync);
	connect(HistoryDB_Mgr::getInstance().get(), &HistoryDB_Mgr::sig_recentChatUserListReady, this, &RecentChatListModel::slot_the_recent_user_list_loading_done);
	connect(this, &RecentChatListModel::loading_user_icon, the_user_icon_mgr::getInstance().get(), &the_user_icon_mgr::get_user_icon_async);// 连接异步加载用户头像信号
	connect(the_user_icon_mgr::getInstance().get(), &the_user_icon_mgr::sig_notify_the_user_icon_loading_done, this, &RecentChatListModel::slot_the_user_icon_loading_done);// 连接异步加载用户头像完成信号
	connect(this, &RecentChatListModel::sig_upsert_user, this, &RecentChatListModel::slot_upsert_user);
	// 启动时加载最近联系人列表
	emit sig_the_recent_user_list_loading();
	emit loading_user_icon(get_user_id_list());//启动时加载所有用户头像
}

void RecentChatListModel::slot_upsert_user(const QString& user_id, const QString& avatarPath, const QString& last_msg, int unread)
{
	// 查找是否存在
	int found = -1;
	qDebug() << "更新或插入用户:" << user_id << avatarPath << last_msg << unread << __FILE__ << __LINE__;
	for (int i = 0; i < model.size(); ++i) {
		if (model[i].get_user_id() == user_id) { found = i; break; }
	}
	if (found >= 0) {
		// 更新
		auto &info = model[found];
		info.set_last_message(last_msg);
		info.set_last_message_time(QDateTime::currentDateTime());
		info.set_unread_message_count(unread);
		
		// 只在user_id_list中不存在时才添加
		if (!user_id_list.contains(user_id)) {
			user_id_list.append(user_id);
		}
		
		// 如果头像路径不为空，加载QPixmap并设置
		if (!avatarPath.isEmpty()) {
			QPixmap avatar(avatarPath);
			if (!avatar.isNull()) {
				info.set_user_avatar(avatar);
			}
		}
		QModelIndex idx = index(found, 0);
		emit dataChanged(idx, idx, {UserNameRole, LastMessageRole, LastMessageTimeRole, UnreadMessageCountRole, UserAvatarRole});
	} else {
		// 插入
		beginInsertRows(QModelIndex(), model.size(), model.size());
		
		// 创建QPixmap对象
		QPixmap avatar;
		if (!avatarPath.isEmpty()) {
			avatar = QPixmap(avatarPath);
		}
		
		the_connected_user_info info(user_id, "", avatar);
		info.set_last_message(last_msg);
		info.set_last_message_time(QDateTime::currentDateTime());
		info.set_unread_message_count(unread);
		
		// 只在user_id_list中不存在时才添加
		if (!user_id_list.contains(user_id)) {
			user_id_list.append(user_id);
		}
		
		model.push_back(info);
		endInsertRows();
		qDebug() << "现在的model大小:" << model.size();
		
		// 自动保存到数据库
		HistoryDB_Mgr::getInstance()->updateRecentContact(user_id, "", last_msg);
	}

	// 自动更新总未读消息数
    updateTotalUnreadMessageCount();
	
	// 增加或刷新权重
	increase_chat_weight(user_id);
	emit this->sig_request_users_name(this->user_id_list);
}

void RecentChatListModel::slot_user_selected(QString const& user_id)
{
	// 用户选择聊天: 提升权重, 确保缓存存在
	increase_chat_weight(user_id);
	add_the_model_cache(user_id);
	qDebug() << "User selected, increased weight and ensured cache for user_id:" << user_id;
	
}

void RecentChatListModel::add_the_model_cache(QString const& user_id)
{
	//添加一个model到缓存中
	if (model_cache->contains(user_id))
	{
		return;
	}
	//如果缓存中没有该model则创建一个新的model并添加到缓存中
	model_cache->insert(user_id, new ChatHistoryDataModel(this));
	was_touched_chat.insert(user_id, true);//标记为被触碰过
}


void RecentChatListModel::slot_clear_all_unread_message()
{
	//清除所有未读消息的数量
	for (auto&item:this->model)
		item.clear_unread_message_count();
	
	// 自动更新总未读消息数（应该是0）
    updateTotalUnreadMessageCount();
    
    // 通知所有行的数据已更改
    if (!model.isEmpty()) {
        emit dataChanged(index(0, 0), index(model.size() - 1, 0), {UnreadMessageCountRole});
    }
}


void RecentChatListModel::auto_remove_the_cache()
{
	//自动移除一个长期未使用的model
	if (the_chat_weight.isEmpty())
		return;
	QString minKey;
	int minValue = std::numeric_limits<int>::max();
	for (auto it = the_chat_weight.constBegin(); it != the_chat_weight.constEnd(); ++it) {
		if (it.value() < minValue) {
			minValue = it.value();
			minKey = it.key();
		}
	}
	if (!minKey.isEmpty()) {
		remove_the_cache(minKey);
		was_touched_chat.remove(minKey);
	}
}
void RecentChatListModel::remove_the_cache(QString const& user_id)
{
	//指定移除一个model
	if (model_cache->contains(user_id))
	{
		auto model = model_cache->take(user_id);
		delete model; // ChatHistoryDataModel 直接删除
	}
	the_chat_weight.remove(user_id);
}

void RecentChatListModel::save_all_cache()
{
	// QCache 没有直接 keys() 的安全遍历, 依赖权重表中的键来尝试访问
	for (auto it = the_chat_weight.constBegin(); it != the_chat_weight.constEnd(); ++it) {
		const QString &key = it.key();
		if (model_cache->contains(key)) {
			auto *chat_model = model_cache->object(key);
			if (chat_model) {
				// TODO: 触发该 model 的脏数据持久化 (若需要)
			}
		}
	}
}

void RecentChatListModel::write_the_chat_weight()
{
	//写入热度表
	QDir dir(data_dir);
	if (!dir.exists()) {
		dir.mkpath(".");
	}
	QString file_path = dir.filePath("the_chat_weight.dat");
	QFile file(file_path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		qWarning() << u8"无法打开热度表进行写入:" << file.errorString() << " path=" << file_path;
		return;
	}
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_6_0);
	out << the_chat_weight;
}


unsigned char RecentChatListModel::get_all_unread_message_count()
{
	//获取所有未读消息的数量
	updateTotalUnreadMessageCount();
    return total_unread_message_count;
}


void RecentChatListModel::clear_the_unread_message_count(const QModelIndex& index)
{
	auto row = index.row();
	if (row < 0 || row >= model.size())
		return;
	auto& info = model[row];
	info.clear_unread_message_count();
	emit dataChanged(index, index, { UnreadMessageCountRole });
	
	// 自动更新总未读消息数
    updateTotalUnreadMessageCount();
}



void RecentChatListModel::increase_chat_weight(QString const& user_id)
{
	//增加某个聊天的热度值
	if (the_chat_weight.contains(user_id))
	{
		the_chat_weight[user_id]++;
		was_touched_chat[user_id] = true;//标记为被触碰过
	}
	else
	{
		the_chat_weight[user_id] = 1;
		//新聊天，初始权重为1并且添加到缓存
		add_the_model_cache(user_id);
		if (!was_touched_chat.contains(user_id))
			was_touched_chat.insert(user_id, true);
	}
	emit the_weight_was_increment(user_id);
}

void RecentChatListModel::check_the_last_touch_chat_and_weight()
{
	//检查一段时间内没有被触碰过的聊天并将其权重值减1
	auto it = was_touched_chat.begin();
	while (it != was_touched_chat.end())
	{
		if (!it.value()) // 如果没有被触碰过
		{
			const QString user_id = it.key();
			if (the_chat_weight.contains(user_id))
			{
				int &w = the_chat_weight[user_id];
				--w;
				if (w <= 0)
				{
					remove_the_cache(user_id);
					it = was_touched_chat.erase(it);
					continue;
				}
			}
		}
		else
		{
			it.value() = false; // 重置为未触碰状态
		}
		++it;
	}
}

RecentChatListModel::~RecentChatListModel()
{
	write_the_chat_weight();
	RecentChatListModel::save_all_cache();
	if (!HistoryDB_Mgr::getInstance()->storeRecentUserList(this->model))
		{
		qWarning() << u8"无法保存最近联系人列表";
		HistoryDB_Mgr::getInstance()->storeRecentUserList(this->model);// 尝试再保存一次
	}
	qDebug() << u8"写入成功！";
	delete model_cache;
	model_cache = nullptr;
}

QVariant RecentChatListModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= model.size())
		return {};
	const the_connected_user_info& user_info = model.at(index.row());
	switch (role)
	{
	case UserIdRole:
		return user_info.get_user_id();
	case UserNameRole:
		return user_info.get_user_name();
	case UserAvatarRole:
		return user_info.get_user_avatar();
	case LastMessageRole:
		return user_info.get_last_message();
	case LastMessageTimeRole:
		return user_info.get_last_message_time();
	case UnreadMessageCountRole:
		return user_info.get_unread_message_count();
	default:
		return {};
	}
}


int RecentChatListModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;
	return model.size();
}



ChatHistoryDataModel* RecentChatListModel::get_model_from_cache(QString const& user_id)
{
	//获取某个用户的聊天记录model
	if (model_cache->contains(user_id))
	{
		return model_cache->object(user_id);
	}
	return nullptr;
}



QList<QString>& RecentChatListModel::get_user_id_list()
{
	//获取所有用户的ID列表
	return user_id_list;
}



void RecentChatListModel::slot_the_user_icon_loading_done(const QList<std::pair<QPixmap,QString>>& icon_list)
{
	for (const auto& item : icon_list)
	{
		const QPixmap& icon = item.first;
		const QString& user_id = item.second;
		// 更新对应用户的头像
		for (int i = 0; i < model.size(); ++i)
		{
			if (model[i].get_user_id() == user_id)
			{
				model[i].set_user_avatar(icon);
				QModelIndex idx = index(i, 0);
				emit dataChanged(idx, idx, { UserAvatarRole });
				break;
			}
		}
	}
}


void RecentChatListModel::slot_the_recent_user_list_loading_done(const QList<the_connected_user_info>& user_list)
{
	qDebug() << "slot_the_recent_user_list_loading_done called with" << user_list.size() << "users";
    
    // 如果用户列表为空，但我们可能有手动添加的用户，所以不要清空
    if (user_list.isEmpty() && model.isEmpty()) {
        qDebug() << "User list is empty and model is empty, nothing to update";
        return;
    }
    
    // 保存现有的手动添加的用户
    QList<the_connected_user_info> existingUsers = model;
    
    // 清空现有数据并重新填充
    beginResetModel();
    
    model.clear();
    user_id_list.clear();
    
    // 首先添加数据库中的用户
    for (const auto& item : user_list) {
        qDebug() << "Adding user from database to model:" << item.get_user_id() << item.get_user_name();
        
        // 直接添加到 model
        the_connected_user_info userInfo = item;
        model.append(userInfo);
        user_id_list.append(item.get_user_id());
        
        // 增加权重但不触发信号
        if (!the_chat_weight.contains(item.get_user_id())) {
            the_chat_weight[item.get_user_id()] = 1;
            was_touched_chat[item.get_user_id()] = false;
        }
    }
    
    // 然后添加不在数据库中但存在于内存中的用户（手动添加的用户）
    for (const auto& existingUser : existingUsers) {
        bool found = false;
        for (const auto& dbUser : user_list) {
            if (existingUser.get_user_id() == dbUser.get_user_id()) {
                found = true;
                break;
            }
        }
        
        // 如果现有用户不在数据库列表中，说明是手动添加的，保留它
        if (!found) {
            qDebug() << "Preserving manually added user:" << existingUser.get_user_id();
            model.append(existingUser);
            if (!user_id_list.contains(existingUser.get_user_id())) {
                user_id_list.append(existingUser.get_user_id());
            }
            
            // 确保权重存在
            if (!the_chat_weight.contains(existingUser.get_user_id())) {
                the_chat_weight[existingUser.get_user_id()] = 1;
                was_touched_chat[existingUser.get_user_id()] = false;
            }
        }
    }
    
    updateTotalUnreadMessageCount();
    
    qDebug() << "Model reset complete. Model size:" << model.size();
    qDebug() << "Row count:" << rowCount();
    qDebug() << "Total unread messages:" << total_unread_message_count;
    
    // 异步加载用户头像
    qDebug() << "Total unread messages:" << total_unread_message_count;
    
    // 异步加载用户头像
    if (!user_id_list.isEmpty()) {
        qDebug() << "Loading user icons for" << user_id_list.size() << "users";
        emit loading_user_icon(user_id_list);
    }
	this->loading_users_name();//加载用户名称
}

void RecentChatListModel::setTotalUnreadMessageCount(unsigned char count)
{
    if (total_unread_message_count != count) {
        total_unread_message_count = count;
        emit totalUnreadMessageCountChanged(count);
        emit sig_the_unread_message_count_change(count); // 保持向后兼容
        qDebug() << "Total unread message count changed to:" << count;
    }
}

void RecentChatListModel::updateTotalUnreadMessageCount()
{
    unsigned char newTotal = 0;
    for (const auto& info : model) {
        newTotal += info.get_unread_message_count();
    }
    setTotalUnreadMessageCount(newTotal);
}

void RecentChatListModel::slot_the_recent_user_name_loading_done(const QList<std::pair<QString, QString>>& users_name)
{
	//更新用户名称
	for (auto& i:users_name)
		qDebug() << "Loaded user name for user_id:" << i.first << " name:" << i.second;
	for (const auto& user : users_name)
	{
		auto user_id = user.first;
		auto new_name = user.second;
		for (int i = 0; i < static_cast<int>(this->model.size()); ++i)
		{
			auto& item = this->model[i];
			if (item.get_user_id() == user_id)
			{
				item.set_user_name(new_name);
				qDebug() << "Updated user name for user_id:" << user_id << " to " << new_name;
				QModelIndex idx = index(i, 0);
				emit dataChanged(idx, idx, { UserNameRole });
				break; // 找到后跳出内层循环
			}
		}
		qDebug() << "Processed user name for user_id:" << user_id << " new name:" << new_name;
	}
}


void RecentChatListModel::slot_the_user_name_changed(const std::pair<QString, QString>& user)
{
	//如果用户名称改变则更新
	auto user_id = user.first;
	auto new_name = user.second;
	
	for(int i=0;i<static_cast<int>( this->model.size());++i)
	{
		auto& item = this->model[i];
		if (item.get_user_id()==user_id)
		{
			item.set_user_name(new_name);
			QModelIndex idx = index(i, 0);
			emit dataChanged(idx, idx, { UserNameRole });
		}
	}
}

void RecentChatListModel::loading_users_name()
{
    if (user_id_list.isEmpty()) {
        qDebug() << "RecentChatListModel: No user IDs to load names for";
        return;
    }
    
    qDebug() << "RecentChatListModel: Requesting user names for" << user_id_list.size() << "users:" << user_id_list;
    emit sig_request_users_name(user_id_list);
}
