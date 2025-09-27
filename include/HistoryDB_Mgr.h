#ifndef HISTORY_DB_MGR_H
#define HISTORY_DB_MGR_H
#include "IChatHistoryProvider.h"
#include <QSqlDatabase>
#include <QApplication>
#include <QDir>
#include "singleton.h"

class HistoryDB_Mgr:public IChatHistoryProvider ,public singleton<HistoryDB_Mgr>
{
    Q_OBJECT
private:
	friend class singleton<HistoryDB_Mgr>;
	std::mutex db_mutex;
    QSqlDatabase chat_history_databases;
    void init_db();
    static QVariant be_base64_the_message(QVariant message);//对图片或者视频进行base64编码
    static QVariant be_QVariant_from_base64(QByteArray& data);//对图片或者视频进行解码
    HistoryDB_Mgr();
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
 signals:
	 void sig_addMessage(const QString& userId, const QList<ChatMessage>& messages);

public slots:
	void slot_addMessage(const QString& userId, const QList<ChatMessage>& messages);
};
#endif

