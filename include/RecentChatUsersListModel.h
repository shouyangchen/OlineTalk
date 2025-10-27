#ifndef RECENTCHATUSERSLISTMODEL_H
#define RECENTCHATUSERSLISTMODEL_H
#include <QAbstractListModel>
#include <QTimer>
#include <QCache>
#include <QString>
#include <QPixmapCache>
#include "ChatHistoryDataModel.h"
#include <QFutureWatcher>
#include <QtConcurrent>

//新增异步获取头像功能并且异步缓存头像

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
    Q_PROPERTY(unsigned char totalUnreadMessageCount READ getTotalUnreadMessageCount WRITE setTotalUnreadMessageCount NOTIFY totalUnreadMessageCountChanged)

private:
	friend class the_connected_user_info;// 友元类方便访问私有成员
    friend class ChatHistoryView;
	QList<the_connected_user_info>model;// 聊天用户列表
    QList<QString>user_id_list;
    QCache<QString, ChatHistoryDataModel>* model_cache { nullptr }; // 缓存model
    QMap<QString,int> the_chat_weight;                           // 每个聊天的热度值
	QHash<QString,bool>was_touched_chat;//记录一段时间内被触碰过的聊天
	QTimer* timer { nullptr }; //用于在一段时间内热点不变时自动移除最冷的聊天
    QString data_dir; // 数据持久化目录（非 UI 线程也可安全使用）
	unsigned char total_unread_message_count{ 0 }; // 所有聊天的未读消息总数
    QList<QString>& get_user_id_list();
    
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
    
    // Q_PROPERTY 的 getter 和 setter
    unsigned char getTotalUnreadMessageCount() const { return total_unread_message_count; }
    void setTotalUnreadMessageCount(unsigned char count);
    
    // 辅助方法：重新计算并更新总未读消息数
    void updateTotalUnreadMessageCount();
    
    void add_the_model_cache(QString const& user_id) override;
    void auto_remove_the_cache() override;
    void remove_the_cache(QString const& user_id) override;
    void save_all_cache()override;
    void write_the_chat_weight();
    unsigned char get_all_unread_message_count();
    void clear_the_unread_message_count(const QModelIndex& index);
    void check_the_last_touch_chat_and_weight();
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	ChatHistoryDataModel* get_model_from_cache(QString const& user_id);//根据用户ID获取聊天记录model
    ~RecentChatListModel() override;

    // 用于调试/预置数据: 插入或更新一个最近联系人

	void loading_user_icon_f() { emit loading_user_icon(this->user_id_list); } // 加载所有用户头像用于测试
    void loading_users_name();//加载所有用户名称
    
signals:
    void the_weight_was_increment(QString const& user_id);
    void loading_recent_user_list();
	void loading_user_icon(const QList<QString> user_list);// 请求异步加载用户头像
	void sig_the_recent_user_list_loading();//请求异步加载最近联系人列表
    void sig_the_unread_message_count_change(const unsigned char&);
	void sig_request_users_name(const QList<QString>& user_id_list);//请求加载用户名称
    void  sig_upsert_user(const QString& user_id, const QString& avatarPath, const QString& last_msg, int unread);
    // Q_PROPERTY 的通知信号
    void totalUnreadMessageCountChanged(unsigned char newCount);
    
public slots:
	void increase_chat_weight(QString const& user_id); // 增加某个聊天的热度值
	void slot_user_selected(QString const& user_id); // 用户选择了某个聊天
    void slot_clear_all_unread_message();
	void slot_the_user_icon_loading_done(const QList<std::pair<QPixmap,QString>>& icon_list);
	void slot_the_recent_user_list_loading_done(const QList<the_connected_user_info>& user_list);
	void slot_the_recent_user_name_loading_done(const QList<std::pair<QString, QString>>& users_name);
	void slot_the_user_name_changed(const std::pair<QString, QString>& user_name);
    void slot_upsert_user(const QString& user_id, const QString& avatarPath, const QString& last_msg, int unread = 0);
	void slot_new_message_coming(ChatMessage message, QString from_uid);// 有新的消息到达
};

#endif // RECENTCHATUSERSLISTMODEL_H