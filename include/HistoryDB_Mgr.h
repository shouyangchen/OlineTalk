#ifndef HISTORY_DB_MGR_H
#define HISTORY_DB_MGR_H
#include "IChatHistoryProvider.h"
#include "global.h"
#include <QSqlDatabase>
#include <QApplication>
#include <QDir>
#include "singleton.h"
#include <QFutureWatcher>

#include "ConnectUserListModel.h"


class HistoryDB_Mgr:public IChatHistoryProvider ,public singleton<HistoryDB_Mgr>
{
    Q_OBJECT
private:
	friend class singleton<HistoryDB_Mgr>;
	std::mutex db_mutex;
    QSqlDatabase chat_history_databases;
	QFutureWatcher<QList<the_connected_user_info>> future_watcher;// 监视异步任务用于获取最近联系人列表
	QFutureWatcher<QList<connectUserList::user_info>> connect_user_list_future_watcher;//监视异步任务用于获取连接用户列表
	QFutureWatcher<QList<NewFriendApplicationlNS::user_application>>friend_application_future_watcher;//监视异步任务用于获取好友申请列表
    void init_db();
    static QVariant be_base64_the_message(QVariant message);//对图片或者视频进行base64编码
    static QVariant be_QVariant_from_base64(QByteArray& data);//对图片或者视频进行解码
    HistoryDB_Mgr();
    void addTestConnectUsersData(); // 添加测试连接用户数据
public:
    HistoryDB_Mgr(HistoryDB_Mgr&&) = delete;
    HistoryDB_Mgr(HistoryDB_Mgr&) = delete;
    HistoryDB_Mgr& operator=(const HistoryDB_Mgr&) = delete;

    // 获取历史聊天记录
     QList<ChatMessage> getHistory(const QString& userId, int offset, int count)override;

	 //获取当前模型最早的聊天记录时间戳更早的记录
	 QList<ChatMessage> get_more_earlier_history(const QString& userId, const QDateTime& earliestTimestamp, int count);
    // 增加新的聊天记录
    bool addMessage(const QString& userId, const ChatMessage& message) override;

    // 获取总数
    int getHistoryCount(const QString& userId) override;

	QList<QString> getUserIdFromDataBase() override;

	void getRecentChatUserListAsync();//异步获取最近联系人列表

	QSharedPointer<QPromise<QList<the_connected_user_info>>>  getRecentChatUserList();//获取最近联系人列表的Promise

	void updateRecentContact(const QString& userId, const QString& userName, const QString& lastMessage);//更新最近联系人列表

	void clearUnreadCount(const QString& userId);//清除未读消息数

	bool storeRecentUserList(const QList<the_connected_user_info>& userList);//存储最近联系人列表

    void getConnectUserListAsync();

    void updateConnectUserList(const connectUserList::user_info& info);//更新联系人列表

    QSharedPointer<QPromise<QList<connectUserList::user_info>>> getConnectUserList();

	QFuture<QList<NewFriendApplicationlNS::user_application>>getApplicationList();//获取好友申请列表的Promise

	void getApplicationListAsync();//异步获取好友申请列表

	bool storeApplicationList(const QList<NewFriendApplicationlNS::user_application>& application_list);//存储好友申请列表

	~HistoryDB_Mgr()override=default;
 signals:
	 void sig_addMessage(const QString& userId, const QList<ChatMessage>& messages);

     void sig_recentChatUserListReady(const QList<the_connected_user_info>&);

	 void sig_request_connect_user_list();//请求联系人列表

	 void sig_connectUserListReady(const QList<connectUserList::user_info>&);//通知联系用户列表已经准备好

	 void sig_applicationListReady(const QList<NewFriendApplicationlNS::user_application>&);//通知好友申请列表已经准备好

	 void sig_requestApplicationsList();

	 void sig_storeApplicationList(const QList<NewFriendApplicationlNS::user_application>& application_list);//存储好友申请列表
public slots:
	void slot_addMessage(const QString& userId, const QList<ChatMessage>& messages);
	void slot_update_connect_user_list();//更新联系人列表
	void slot_storeApplicationList(const QList<NewFriendApplicationlNS::user_application>& application_list);//存储好友申请列表
};
#endif

