#include "RecentChatUsersListModel.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <algorithm>
#include <QThread>

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
}

void RecentChatListModel::upsert_user(const QString& user_id, const QString& name, const QString& avatarPath, const QString& last_msg, int unread)
{
	// 查找是否存在
	int found = -1;
	for (int i = 0; i < model.size(); ++i) {
		if (model[i].get_user_id() == user_id) { found = i; break; }
	}
	if (found >= 0) {
		// 更新
		auto &info = model[found];
		info.set_user_name(name);
		info.set_last_message(last_msg);
		info.set_last_message_time(QDateTime::currentDateTime());
		info.set_unread_message_count(unread);
		if (!avatarPath.isEmpty())
			info.set_user_avatar(avatarPath);
		QModelIndex idx = index(found, 0);
		emit dataChanged(idx, idx, {UserNameRole, LastMessageRole, LastMessageTimeRole, UnreadMessageCountRole, UserAvatarRole});
	} else {
		// 插入
		beginInsertRows(QModelIndex(), model.size(), model.size());
		the_connected_user_info info(user_id, name,QPixmap(avatarPath));
		info.set_last_message(last_msg);
		info.set_last_message_time(QDateTime::currentDateTime());
		info.set_unread_message_count(unread);
		model.push_back(info);
		endInsertRows();
	}
	// 增加或刷新权重
	increase_chat_weight(user_id);
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
	save_all_cache();
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


// ------------------ the_connected_user_info 实现 ------------------

the_connected_user_info::the_connected_user_info(QString const& id, QString const& name, QPixmap const& avatar)
	: user_id(id), user_name(name), user_avatar(avatar)
{}

const QString& the_connected_user_info::get_user_id() const { return user_id; }
const QString& the_connected_user_info::get_user_name() const { return user_name; }
QPixmap the_connected_user_info::get_user_avatar() const { return user_avatar; }
const QString& the_connected_user_info::get_last_message() const { return last_message; }
const QDateTime& the_connected_user_info::get_last_message_time() const { return last_message_time; }
int the_connected_user_info::get_unread_message_count() const { return unread_message_count; }
void the_connected_user_info::set_user_name(QString const& name) { user_name = name; }
void the_connected_user_info::set_user_avatar(QPixmap const& avatar) { user_avatar = avatar; }
void the_connected_user_info::set_last_message(QString const& message) { last_message = message; }
void the_connected_user_info::set_last_message_time(QDateTime const& time) { last_message_time = time; }
void the_connected_user_info::set_unread_message_count(int count) { unread_message_count = count; }
void the_connected_user_info::increment_unread_message_count() { ++unread_message_count; }
void the_connected_user_info::clear_unread_message_count() { unread_message_count = 0; }



