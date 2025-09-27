#include "HistoryDB_Mgr.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QFile>
#include <mutex>

HistoryDB_Mgr::HistoryDB_Mgr() :IChatHistoryProvider()
{
    QString appPath = QApplication::applicationDirPath();
    QString dbDir = appPath + "/user_chat_database";
    QString dbPath = dbDir + "/chat_history.db";
    if (!QDir(dbDir).exists()) {
        QDir().mkpath(dbDir);
    }

    if (!QFile::exists(dbPath)) {
        QFile file(dbPath);
        if (!file.open(QIODevice::WriteOnly)) {
            qDebug() << "Failed to create user chat history database file.";
            return;
        }
        file.close();
    }

    this->chat_history_databases = QSqlDatabase::addDatabase("QSQLITE");
    this->chat_history_databases.setDatabaseName(dbPath);
    if (!this->chat_history_databases.open()) {
        qDebug() << "Failed to open database:" << this->chat_history_databases.lastError().text();
        return;
    }
    else {
        this->init_db();
        qDebug() << "Database created and opened successfully.";
    }
}

void HistoryDB_Mgr::init_db()
{
    if (!this->chat_history_databases.isOpen()) {
        qDebug() << "Database is not open. Cannot initialize.";
        return;
    }
    QSqlQuery query(this->chat_history_databases);
    if (query.exec(
        "CREATE TABLE IF NOT EXISTS chat_history ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "user_id TEXT, "
        "message_content TEXT, "
        "sender TEXT, "
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP, "
        "message_type TEXT)"
    )) {
        qDebug() << "Chat history table created or already exists.";
    }
    else {
        qDebug() << "Failed to create chat history table:" << query.lastError().text();
    }
}

QVariant HistoryDB_Mgr::be_base64_the_message(QVariant message)
{
    return message.toByteArray().toBase64();
}

QVariant HistoryDB_Mgr::be_QVariant_from_base64(QByteArray& data)
{
    return QByteArray::fromBase64(data);
}

QList<ChatMessage> HistoryDB_Mgr::getHistory(const QString& userId, int offset, int count)
{
    QList<ChatMessage> messages;
    if (!this->chat_history_databases.isOpen()) {
        qDebug() << "Database is not open. Cannot get history.";
        return messages;
    }
    QSqlQuery query(this->chat_history_databases);
    // 拼接 LIMIT/OFFSET 参数
	//OFFSET 用于指定从哪一行开始返回结果，LIMIT 用于指定返回多少行
    QString sql = QString("SELECT message_content, sender, timestamp, message_type FROM chat_history WHERE user_id = :userId ORDER BY timestamp DESC LIMIT %1 OFFSET %2").arg(count).arg(offset);//降序
    query.prepare(sql);
    query.bindValue(":userId", userId);
    if (!query.exec()) {
        qDebug() << "Failed to execute query:" << query.lastError().text();
        return messages;
    }
    while (query.next()) {
        ChatMessage msg;
        msg.message_context = query.value(0).toString();
        msg.sender = query.value(1).toString();
        msg.timestamp = query.value(2).toDateTime();
        msg.message_type = query.value(3).toString();
		messages.insert(0, msg);//将最早的消息插入到最前面此时第一个消息是最早的
    }
    return messages;
}

bool HistoryDB_Mgr::addMessage(const QString& userId, const ChatMessage& message)
{
    if (!this->chat_history_databases.isOpen()) {
        qDebug() << "Database is not open. Cannot add message.";
        return false;
    }
    QVariant encoded_message = message.message_context;
    if (message.message_type != "text") {
        encoded_message = this->be_base64_the_message(message.message_context);
    }

    QSqlQuery query(this->chat_history_databases);
    query.prepare("INSERT INTO chat_history (user_id, message_content, sender, timestamp, message_type) VALUES (:userId, :messageContent, :sender, :timestamp, :messageType)");
    query.bindValue(":userId", userId);
    query.bindValue(":messageContent", encoded_message.toString());
    query.bindValue(":sender", message.sender);
    query.bindValue(":timestamp", message.timestamp);
    query.bindValue(":messageType", message.message_type);

    if (!query.exec()) {
        qDebug() << "Failed to insert message:" << query.lastError().text();
        return false;
    }
    return true;
}

int HistoryDB_Mgr::getHistoryCount(const QString& userId)
{
    if (!this->chat_history_databases.isOpen()) {
        qDebug() << "Database is not open. Cannot get history count.";
        return 0;
    }
    QSqlQuery query(this->chat_history_databases);
    query.prepare("SELECT COUNT(*) FROM chat_history WHERE user_id = :userId");
    query.bindValue(":userId", userId);
    if (!query.exec()) {
        qDebug() << "Failed to execute query:" << query.lastError().text();
        return 0;
    }
    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

void HistoryDB_Mgr::slot_addMessage(const QString& userId, const QList<ChatMessage>& messages)
{
    if (messages.isEmpty())
        return;
    std::lock_guard<std::mutex> lock(this->db_mutex);
    if (!this->chat_history_databases.isOpen())
        return;

    this->chat_history_databases.transaction();
    QSqlQuery query(this->chat_history_databases);

    for (const auto& message : messages) {
        QVariant encoded_message = message.message_context;
        if (message.message_type != "text") {
            encoded_message = this->be_base64_the_message(message.message_context);
        }
        query.prepare("INSERT INTO chat_history (user_id, message_content, sender, timestamp, message_type) VALUES (:userId, :messageContent, :sender, :timestamp, :messageType)");
        query.bindValue(":userId", userId);
        query.bindValue(":messageContent", encoded_message.toString());
        query.bindValue(":sender", message.sender);
        query.bindValue(":timestamp", message.timestamp);
        query.bindValue(":messageType", message.message_type);

        if (!query.exec()) {
            qDebug() << "Failed to insert message in batch:" << query.lastError().text();
            this->chat_history_databases.rollback();
            return;
        }
    }
    this->chat_history_databases.commit();
    qDebug() << "add message done!";
}


QList<ChatMessage> HistoryDB_Mgr::get_more_earlier_history(const QString& userId, const QDateTime& earliestTimestamp, int count)
{
	// 获取比 earliestTimestamp 更早的聊天记录
    QList<ChatMessage> messages;
    if (!this->chat_history_databases.isOpen()) {
        qDebug() << "Database is not open. Cannot get history.";
        return messages;
    }
    QSqlQuery query(this->chat_history_databases);
    // 拼接 LIMIT 参数
    // 获取比 earliestTimestamp 更早的记录，按时间降序排列，取前 count 条
    QString sql = QString("SELECT message_content, sender, timestamp, message_type FROM chat_history WHERE user_id = :userId AND timestamp < :earliestTimestamp ORDER BY timestamp DESC LIMIT %1").arg(count);
    query.prepare(sql);
    query.bindValue(":userId", userId);
    query.bindValue(":earliestTimestamp", earliestTimestamp);
    if (!query.exec()) {
        qDebug() << "Failed to execute query:" << query.lastError().text();
        return messages;
    }
    while (query.next()) {
        ChatMessage msg;
        msg.message_context = query.value(0).toString();
        msg.sender = query.value(1).toString();
        msg.timestamp = query.value(2).toDateTime();
        msg.message_type = query.value(3).toString();
        messages.append(msg);
    }
    // 由于是按时间降序取的，需要反转列表以按时间升序返回
    std::reverse(messages.begin(), messages.end());
	qDebug() << "get_more_earlier_history done!";
    return messages;
}

QList<QString> HistoryDB_Mgr::getUserIdFromDataBase()
{
    QList<QString> userIds;
    if (!this->chat_history_databases.isOpen()) {
        qDebug() << "Database is not open. Cannot get user IDs.";
        return userIds;
    }
    QSqlQuery query(this->chat_history_databases);
    QString sql = QString("SELECT DISTINCT user_id FROM chat_history");
    if (!query.exec(sql)) {
        qDebug() << "Failed to execute query:" << query.lastError().text();
        return userIds;
    }
    while (query.next()) {
        QString userId = query.value(0).toString();
        userIds.append(userId);
    }
    return userIds;
}