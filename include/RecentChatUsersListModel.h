#ifndef RECENTCHATUSERSLISTMODEL_H
#define RECENTCHATUSERSLISTMODEL_H
#include <QAbstractListModel>
#include <QTimer>
#include <QCache>
#include <QString>
#include <QPixmapCache>
#include "ChatHistoryDataModel.h"

class RecentChatListModel;

class the_connected_user_info
{
private:
    friend RecentChatListModel;
    QString user_id;
    QString user_name;
	QPixmap user_avatar;// 头像
	QString last_message;// 最后一条消息
	QDateTime last_message_time;// 最后消息时间
    int unread_message_count { 0 };
public:
    the_connected_user_info() = default;
    the_connected_user_info(QString const& id, QString const& name, QPixmap const& avatar);
	const QString& get_user_id() const;
    const QString& get_user_name() const;
    QPixmap get_user_avatar() const;
    const QString& get_last_message() const;
    const QDateTime& get_last_message_time() const;
    int get_unread_message_count() const;
    void set_user_name(QString const& name);
    void set_user_avatar(QPixmap const& avatar);
    void set_last_message(QString const& message);
    void set_last_message_time(QDateTime const& time);
    void set_unread_message_count(int count);
    void increment_unread_message_count();
	void clear_unread_message_count();
	
};
/**
 * 本model要实现的功能：
 * 消息列表缓存与按需加载：
 *  - 维护近期聊天用户对应的 ChatHistoryDataModel
 *  - 通过热度(权重)表 the_chat_weight 选择淘汰最冷的会话 model
 *  - 退出时保存所有仍在缓存中的改动
 * 线程/UI 依赖消除：数据目录采用 QStandardPaths::AppDataLocation，避免直接依赖 QApplication::applicationDirPath() 在主线程调用。
 */

class IRecentChatUsersListModel // 抽象接口
{
public:
    virtual void add_the_model_cache(QString const& user_id) = 0; // 添加一个model到缓存
    virtual void auto_remove_the_cache() = 0;                     // 自动移除一个长期未使用的model
    virtual void remove_the_cache(QString const& user_id) = 0;    // 指定移除
    virtual void save_all_cache() = 0;                            // 保存所有缓存
    virtual ~IRecentChatUsersListModel() = default;
};

class RecentChatListModel : public QAbstractListModel, public IRecentChatUsersListModel
{
    Q_OBJECT
private:
	friend class the_connected_user_info;// 友元类方便访问私有成员
    friend class ChatHistoryView;
	QList<the_connected_user_info>model;// 聊天用户列表
    QCache<QString, ChatHistoryDataModel>* model_cache { nullptr }; // 缓存model
    QMap<QString,int> the_chat_weight;                           // 每个聊天的热度值
	QHash<QString,bool>was_touched_chat;//记录一段时间内被触碰过的聊天
	QTimer* timer { nullptr }; //用于在一段时间内热点不变时自动移除最冷的聊天
    QString data_dir; // 数据持久化目录（非 UI 线程也可安全使用）
public:
    enum Roles {
		UserIdRole = Qt::UserRole + 5,// 从Qt::UserRole+4开始自定义角色,因为前面已经被使用
        UserNameRole,
        UserAvatarRole,
        LastMessageRole,
        LastMessageTimeRole,
        UnreadMessageCountRole
    };
    RecentChatListModel(QObject* parent = nullptr);
	RecentChatListModel(RecentChatListModel const&) = delete;
	RecentChatListModel& operator=(RecentChatListModel const&) = delete;
    void add_the_model_cache(QString const& user_id) override;
    void auto_remove_the_cache() override;
    void remove_the_cache(QString const& user_id) override;
    void save_all_cache()override;
    void write_the_chat_weight();
    void check_the_last_touch_chat_and_weight();
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	ChatHistoryDataModel* get_model_from_cache(QString const& user_id);
    ~RecentChatListModel() override;

    // 用于调试/预置数据: 插入或更新一个最近联系人
    void upsert_user(const QString& user_id, const QString& name, const QString& avatarPath, const QString& last_msg, int unread = 0);

signals:
    void the_weight_was_increment(QString const& user_id);
public slots:
	void increase_chat_weight(QString const& user_id); // 增加某个聊天的热度值
	void slot_user_selected(QString const& user_id); // 用户选择了某个聊天
};

#endif // RECENTCHATUSERSLISTMODEL_H