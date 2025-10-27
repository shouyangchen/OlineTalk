#include "HistoryDB_Mgr.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QPromise>
#include <QFile>
#include <mutex>
#include <qtconcurrentrun.h>

#include "the_user_icon_mgr.h"

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
		is_first_login = true;//标记为第一次登录需要下载用户头像等信息和加载登录用户的联系人列表
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

    // 修正信号连接：连接到正确的异步方法
    connect(this, &HistoryDB_Mgr::sig_request_connect_user_list,
        this, &HistoryDB_Mgr::getConnectUserListAsync);
    connect(this, &HistoryDB_Mgr::sig_requestApplicationsList, [this]
        {
            this->getApplicationListAsync();
		});// 使用lambda捕获this指针调用成员函数
    connect(this, &HistoryDB_Mgr::sig_storeApplicationList, this, &HistoryDB_Mgr::slot_storeApplicationList);
    qDebug() << "HistoryDB_Mgr initialized with signal connections";
}

void HistoryDB_Mgr::init_db()
{
    if (!this->chat_history_databases.isOpen()) {
        qDebug() << "Database is not open. Cannot initialize.";
        return;
    }

    QSqlQuery query(this->chat_history_databases);

    // 创建聊天历史表
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

    // 创建最近联系人列表表
    QSqlQuery createRecentUserListTableQuery(this->chat_history_databases);
    if (createRecentUserListTableQuery.exec(
        "CREATE TABLE IF NOT EXISTS recent_contacts ("
        "user_id TEXT PRIMARY KEY, "
        "last_message TEXT, "
        "unread_count INTEGER, "
        "last_interaction DATETIME DEFAULT CURRENT_TIMESTAMP)"
    )) {
        qDebug() << "Recent contacts table created or already exists.";
    }
    else {
        qDebug() << "Failed to create recent contacts table:" << createRecentUserListTableQuery.lastError().text();
    }

    // 创建连接用户表
    QSqlQuery createConnectUsersTableQuery(this->chat_history_databases);
    if (createConnectUsersTableQuery.exec(
        "CREATE TABLE IF NOT EXISTS connect_users ("
        "user_id TEXT PRIMARY KEY, "
        "user_name TEXT, "
        "email TEXT)"
    )) {
        qDebug() << "Connect users table created or already exists.";

        // 添加一些测试数据（如果表为空）
        addTestConnectUsersData();
    }
    else {
        qDebug() << "Failed to create connect users table:" << createConnectUsersTableQuery.lastError().text();
    }

	//创建好友申请列表表
	QSqlQuery createFriendApplicationTableQuery(this->chat_history_databases);
    if (createFriendApplicationTableQuery.exec(
        "CREATE TABLE IF NOT EXISTS friend_applications ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "user_id TEXT  UNIQUE, "
        "user_name TEXT, "
        "msg TEXT, "
        "time DATETIME DEFAULT CURRENT_TIMESTAMP, "
        "status INTEGER)"
    )) {
        qDebug() << "Friend applications table created or already exists.";
    }
    else {
        qDebug() << "Failed to create friend applications table:" << createFriendApplicationTableQuery.lastError().text();
	}
}

// 添加测试连接用户数据的方法
void HistoryDB_Mgr::addTestConnectUsersData()
{
    QSqlQuery query(this->chat_history_databases);

    // 检查是否已有数据
    if (!query.exec("SELECT COUNT(*) FROM connect_users")) {
        qDebug() << "Failed to count connect_users:" << query.lastError().text();
        return;
    }

    if (query.next() && query.value(0).toInt() > 0) {
        qDebug() << "Connect users table already has data, count:" << query.value(0).toInt();
        return;
    }

    qDebug() << "Adding test data to connect_users table...";

    // 添加测试数据
    QStringList testUsers = {
        "INSERT INTO connect_users (user_id, user_name, email) VALUES ('userid001', '联系人一', 'user1@example.com')",
        "INSERT INTO connect_users (user_id, user_name, email) VALUES ('userid002', '联系人二', 'user2@example.com')",
        "INSERT INTO connect_users (user_id, user_name, email) VALUES ('userid003', '联系人三', 'user3@example.com')",
        "INSERT INTO connect_users (user_id, user_name, email) VALUES ('userid004', '联系人四', 'user4@example.com')",
        "INSERT INTO connect_users (user_id, user_name, email) VALUES ('userid005', '联系人五', 'user5@example.com')"
    };

    this->chat_history_databases.transaction();
    bool success = true;

    for (const QString& sql : testUsers) {
        if (!query.exec(sql)) {
            qDebug() << "Failed to insert test connect user:" << query.lastError().text();
            success = false;
            break;
        }
    }

    if (success) {
        this->chat_history_databases.commit();
        qDebug() << "Test connect users data inserted successfully";
    }
    else {
        this->chat_history_databases.rollback();
        qDebug() << "Rolling back connect users test data";
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
    //qDebug() << "get_more_earlier_history done!";
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

QSharedPointer<QPromise<QList<the_connected_user_info>>> HistoryDB_Mgr::getRecentChatUserList()
{
    auto promise = QSharedPointer<QPromise<QList<the_connected_user_info>>>(new QPromise<QList<the_connected_user_info>>());

    // 使用 QtConcurrent::run 在后台线程执行数据库查询
    auto future = QtConcurrent::run([this, promise]() {
        QList<the_connected_user_info> userList;
        qDebug() << "Starting database query in background thread...";

        try {
            // 创建线程安全的数据库连接 - 修复版本
            QString connectionName = QString("RecentChatDB_%1").arg(reinterpret_cast<quintptr>(QThread::currentThread()));
            //qDebug() << "Using connection name:" << connectionName;

            QSqlDatabase db;
            if (QSqlDatabase::contains(connectionName)) {
                db = QSqlDatabase::database(connectionName);
                //qDebug() << "Reusing existing database connection";
            }
            else {
                // 不使用 cloneDatabase，直接创建新连接
                db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
                db.setDatabaseName(this->chat_history_databases.databaseName());
                //qDebug() << "Created new database connection with path:" << db.databaseName();
            }

            if (!db.isValid()) {
                //qDebug() << "Database connection is not valid";
                promise->addResult(userList);
                promise->finish();
                return;
            }

            if (!db.open()) {
                //qDebug() << "Failed to open database for recent chat list:" << db.lastError().text();
                promise->addResult(userList);
                promise->finish();
                return;
            }

            //qDebug() << "Database opened successfully, driver:" << db.driverName();

            // 首先检查表是否存在且有数据
            QSqlQuery checkQuery(db);
            if (!checkQuery.exec("SELECT COUNT(*) FROM recent_contacts")) {
                qDebug() << "Failed to count recent contacts:" << checkQuery.lastError().text();

            }
            else if (checkQuery.next()) {
                int count = checkQuery.value(0).toInt();
                qDebug() << "Recent contacts table has" << count << "records";

                if (count == 0) {
                    qDebug() << "No data found, adding test data...";
                }
            }

            QSqlQuery query(db);
            // 查询最近联系人，按最后交互时间降序排列
            QString sql = R"(
                SELECT user_id, last_message, unread_count, last_interaction
                FROM recent_contacts
                ORDER BY last_interaction DESC
            )";

            //qDebug() << "Executing query:" << sql;

            if (!query.exec(sql)) {
                qDebug() << "Failed to execute recent contacts query:" << query.lastError().text();
                promise->addResult(userList);
                promise->finish();
                return;
            }

            qDebug() << "Query executed successfully";

            int count = 0;
            while (query.next()) {
                the_connected_user_info userInfo;
                userInfo.set_user_id(query.value(0).toString());
                userInfo.set_last_message(query.value(1).toString());
                userInfo.set_unread_message_count(query.value(2).toInt());
                userInfo.set_last_message_time(query.value(3).toDateTime());
                userList.append(userInfo);
                count++;

                qDebug() << "Found user:" << userInfo.get_user_id() << userInfo.get_user_name();
            }

            qDebug() << "Total users found:" << count;

            promise->addResult(userList);
            promise->finish();
            qDebug() << "getRecentChatUserList done, found" << userList.size() << "users.";
        }
        catch (const std::exception& e) {
            qDebug() << "Exception in getRecentChatUserList:" << e.what();
            promise->addResult(userList);
            promise->finish();
        }
        catch (...) {
            qDebug() << "Unknown exception in getRecentChatUserList";
            promise->addResult(userList);
            promise->finish();
        }
        });

    return promise;
}

void HistoryDB_Mgr::getRecentChatUserListAsync()
{
    //qDebug() << "Starting getRecentChatUserListAsync...";
    auto promise = this->getRecentChatUserList();
    auto future = promise->future();

    // 先设置 future，再连接信号
    this->future_watcher.setFuture(future);

    // 断开之前的连接，避免重复连接
    disconnect(&this->future_watcher, &QFutureWatcher<QList<the_connected_user_info>>::finished, nullptr, nullptr);

    connect(&this->future_watcher, &QFutureWatcher<QList<the_connected_user_info>>::finished, [this]() {
        //qDebug() << "Future watcher finished signal received";
        try {
            if (this->future_watcher.isFinished()) {
                auto result = this->future_watcher.result();
                qDebug() << "Recent chat user list loaded, count:" << result.size();

                // 发射信号通知UI更新
                emit sig_recentChatUserListReady(result);
            }
            else {
                qDebug() << "Future is not finished yet";
            }
        }
        catch (const std::exception& e) {
            qDebug() << "Error getting recent chat user list result:" << e.what();
        }
        catch (...) {
            qDebug() << "Unknown error in future watcher";
        }
        });
}

void HistoryDB_Mgr::updateRecentContact(const QString& userId, const QString& userName, const QString& lastMessage)
{
    std::lock_guard<std::mutex> lock(this->db_mutex);

    if (!this->chat_history_databases.isOpen()) {
        qDebug() << "Database is not open. Cannot update recent contact.";
        return;
    }

    QSqlQuery query(this->chat_history_databases);

    // 使用 REPLACE 来插入或更新记录
    query.prepare(R"(
        REPLACE INTO recent_contacts (user_id, user_name, last_message, unread_count, last_interaction)
        VALUES (:userId, :lastMessage, 
                COALESCE((SELECT unread_count FROM recent_contacts WHERE user_id = :userId), 0) + 1,
                CURRENT_TIMESTAMP)
    )");

    query.bindValue(":userId", userId);
    query.bindValue(":lastMessage", lastMessage);

    if (!query.exec()) {
        qDebug() << "Failed to update recent contact:" << query.lastError().text();
    }
}

void HistoryDB_Mgr::clearUnreadCount(const QString& userId)
{
    std::lock_guard<std::mutex> lock(this->db_mutex);

    if (!this->chat_history_databases.isOpen()) {
        return;
    }

    QSqlQuery query(this->chat_history_databases);
    if (!userId.isEmpty())// 清除指定用户的未读计数
    {
        query.prepare("UPDATE recent_contacts SET unread_count = 0 WHERE user_id = :userId");
        query.bindValue(":userId", userId);
    }
    else// 清除所有用户的未读计数
    {
        query.prepare("UPDATE recent_contacts SET unread_count = 0");
    }
    if (!query.exec()) {
        qDebug() << "Failed to clear unread count:" << query.lastError().text();
    }
}

bool HistoryDB_Mgr::storeRecentUserList(const QList<the_connected_user_info>& userList)
{
    std::lock_guard<std::mutex> lock(this->db_mutex);
    if (!this->chat_history_databases.isOpen()) {
        qDebug() << "Database is not open. Cannot store recent user list.";
        return false;
    }
    QSqlQuery query(this->chat_history_databases);
    if (!this->chat_history_databases.transaction()) {
        qDebug() << "Failed to start transaction:" << this->chat_history_databases.lastError().text();
        return false;
    }
    bool success = true;
    for (const auto& user : userList) {
        query.prepare(R"(
            INSERT INTO recent_contacts (user_id, last_message, unread_count, last_interaction)
            VALUES (:userId, :lastMessage, :unreadCount, :lastInteraction)
            ON CONFLICT(user_id) DO UPDATE SET
                last_message=excluded.last_message,
                unread_count=excluded.unread_count,
                last_interaction=excluded.last_interaction
        )");
        query.bindValue(":userId", user.get_user_id());
        query.bindValue(":lastMessage", user.get_last_message());
        query.bindValue(":unreadCount", user.get_unread_message_count());
        query.bindValue(":lastInteraction", user.get_last_message_time().toString(Qt::ISODate));
        if (!query.exec()) {
            qDebug() << "Failed to upsert recent contact:" << query.lastError().text()
                << "SQL:" << query.lastQuery()
                << "userId:" << user.get_user_id();
            success = false;
            break;
        }
    }
    if (success) {
        if (!this->chat_history_databases.commit()) {
            qDebug() << "Failed to commit transaction:" << this->chat_history_databases.lastError().text();
            return false;
        }
    }
    else {
        this->chat_history_databases.rollback();
        return false;
    }
    return true;
}

QSharedPointer<QPromise<QList<connectUserList::user_info>>> HistoryDB_Mgr::getConnectUserList()
{
    auto promise = QSharedPointer<QPromise<QList<connectUserList::user_info>>>(new QPromise<QList<connectUserList::user_info>>());

    auto future=QtConcurrent::run([this, promise]() {
        QList<connectUserList::user_info> userList;
        qDebug() << "Starting connect user list query in background thread...";

        try {
            QString connectionName = QString("ConnectUserDB_%1").arg(reinterpret_cast<quintptr>(QThread::currentThread()));
            qDebug() << "Using connection name:" << connectionName;

            QSqlDatabase db;
            if (QSqlDatabase::contains(connectionName)) {
                db = QSqlDatabase::database(connectionName);
                qDebug() << "Reusing existing database connection";
            }
            else {
                db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
                db.setDatabaseName(this->chat_history_databases.databaseName());
                qDebug() << "Created new database connection with path:" << db.databaseName();
            }

            if (!db.isValid() || !db.open()) {
                qDebug() << "Database connection failed:" << db.lastError().text();
                promise->addResult(userList);
                promise->finish();
                return;
            }

            qDebug() << "Database opened successfully, driver:" << db.driverName();

            // 检查表是否存在且有数据
            QSqlQuery checkQuery(db);
            if (!checkQuery.exec("SELECT COUNT(*) FROM connect_users")) {
                qDebug() << "Failed to count connect_users:" << checkQuery.lastError().text();

                // 如果查询失败，可能是表不存在，尝试创建并添加测试数据
                if (checkQuery.exec("CREATE TABLE IF NOT EXISTS connect_users ("
                    "user_id TEXT PRIMARY KEY, "
                    "user_name TEXT, "
                    "email TEXT, "
                    "status TEXT DEFAULT 'online', "
                    "created_time DATETIME DEFAULT CURRENT_TIMESTAMP)")) {

                    // 添加测试数据
                    QStringList testInserts = {
                        "INSERT OR IGNORE INTO connect_users (user_id, user_name, email) VALUES ('connect001', '联系人一', 'user1@example.com')",
                        "INSERT OR IGNORE INTO connect_users (user_id, user_name, email) VALUES ('connect002', '联系人二', 'user2@example.com')",
                        "INSERT OR IGNORE INTO connect_users (user_id, user_name, email) VALUES ('connect003', '联系人三', 'user3@example.com')"
                    };

                    for (const QString& insertSql : testInserts) {
                        QSqlQuery insertQuery(db);
                        if (!insertQuery.exec(insertSql)) {
                            qDebug() << "Failed to insert test data:" << insertQuery.lastError().text();
                        }
                    }
                }
            }
            else if (checkQuery.next()) {
                int count = checkQuery.value(0).toInt();
                //qDebug() << "Connect users table has" << count << "records";
            }

            QSqlQuery query(db);
            QString sql = "SELECT user_id, user_name FROM connect_users ORDER BY user_name ASC";

            qDebug() << "Executing query:" << sql;

            if (!query.exec(sql)) {
                qDebug() << "Failed to execute connect users query:" << query.lastError().text();
                promise->addResult(userList);
                promise->finish();
                return;
            }

            qDebug() << "Query executed successfully";

            int count = 0;
            while (query.next()) {
                connectUserList::user_info userInfo;
                userInfo.user_id = query.value(0).toString();
                userInfo.username = query.value(1).toString();
                // 设置默认头像
                userInfo.avatar = QPixmap(":/res/default_user_icon.png");

                userList.append(userInfo);
                count++;

                qDebug() << "Found connect user:" << userInfo.user_id << userInfo.username;
            }

            qDebug() << "Total connect users found:" << count;

            promise->addResult(userList);
            promise->finish();

        }
        catch (const std::exception& e) {
            qDebug() << "Exception in getConnectUserList:" << e.what();
            promise->addResult(userList);
            promise->finish();
        }
        catch (...) {
            qDebug() << "Unknown exception in getConnectUserList";
            promise->addResult(userList);
            promise->finish();
        }
        });

    return promise;
}

void HistoryDB_Mgr::getConnectUserListAsync()
{
    qDebug() << "Starting getConnectUserListAsync...";
    auto promise = this->getConnectUserList();
    auto future = promise->future();

    // 先设置 future，再连接信号
    this->connect_user_list_future_watcher.setFuture(future);

    // 断开之前的连接，避免重复连接
    disconnect(&this->connect_user_list_future_watcher, &QFutureWatcher<QList<connectUserList::user_info>>::finished, nullptr, nullptr);

    connect(&this->connect_user_list_future_watcher, &QFutureWatcher<QList<connectUserList::user_info>>::finished, [this]() {
        //qDebug() << "Connect user list future watcher finished signal received";
        try {
            // 修复bug：应该检查 connect_user_list_future_watcher 而不是 future_watcher
            if (this->connect_user_list_future_watcher.isFinished()) {
                auto result = this->connect_user_list_future_watcher.result();
                qDebug() << "Connect user list loaded, count:" << result.size();

                // 发射信号通知UI更新
                emit sig_connectUserListReady(result);
            }
            else {
                qDebug() << "Connect user list future is not finished yet";
            }
        }
        catch (const std::exception& e) {
            qDebug() << "Error getting connect user list result:" << e.what();
        }
        catch (...) {
            qDebug() << "Unknown error in connect user list future watcher";
        }
        });
}

void HistoryDB_Mgr::updateConnectUserList(const connectUserList::user_info& info)
{
    std::lock_guard<std::mutex> lock(this->db_mutex);
    if (!this->chat_history_databases.isOpen()) {
        qDebug() << "Database is not open. Cannot update connect user.";
        return;
    }
    QSqlQuery query(this->chat_history_databases);
    // 使用 REPLACE 来插入或更新记录
    query.prepare(R"(
        REPLACE INTO connect_users (user_id, user_name)
        VALUES (:userId, :userName)
    )");
    query.bindValue(":userId", info.user_id);
    query.bindValue(":userName", info.username);
    if (!query.exec()) {
        qDebug() << "Failed to update connect user:" << query.lastError().text();
    }
}

void HistoryDB_Mgr::slot_update_connect_user_list()
{
    qDebug() << "slot_update_connect_user_list called";
    this->getConnectUserListAsync();
}


QFuture<QList<NewFriendApplicationlNS::user_application>> HistoryDB_Mgr::getApplicationList()
{
	auto promise = QSharedPointer<QPromise<QList<NewFriendApplicationlNS::user_application>>>(new QPromise<QList<NewFriendApplicationlNS::user_application>>());
    auto future = QtConcurrent::run([this,promise]() {
        QList<NewFriendApplicationlNS::user_application> applicationList;
        //qDebug() << "Starting friend application list query in background thread...";
        try {
            QFuture<QList<NewFriendApplicationlNS::user_application>>future;
            QString connectionName = QString("FriendApplicationDB_%1").arg(reinterpret_cast<quintptr>(QThread::currentThread()));
            qDebug() << "Using connection name:" << connectionName;
            QSqlDatabase db;
            if (QSqlDatabase::contains(connectionName)) {
                db = QSqlDatabase::database(connectionName);
                qDebug() << "Reusing existing database connection";
            }
            else {
                db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
                db.setDatabaseName(this->chat_history_databases.databaseName());
                qDebug() << "Created new database connection with path:" << db.databaseName();
            }
            if (!db.isValid() || !db.open()) {
                qDebug() << "Database connection failed:" << db.lastError().text();
                promise->addResult(applicationList);
                promise->finish();
                return;
            }
            qDebug() << "Database opened successfully, driver:" << db.driverName();
            // 检查表是否存在且有数据
            QSqlQuery checkQuery(db);
            if (!checkQuery.exec("SELECT COUNT(*) FROM friend_applications")) {
                qDebug() << "Failed to count friend_applications:" << checkQuery.lastError().text();
            }
            else if (checkQuery.next()) {
                int count = checkQuery.value(0).toInt();
                qDebug() << "Friend applications table has" << count << "records";
            }
            QSqlQuery query(db);
            QString sql = "SELECT user_id, user_name, msg, time, status FROM friend_applications ORDER BY time DESC";
            //qDebug() << "Executing query:" << sql;
            if (!query.exec(sql)) {
                qDebug() << "Failed to execute friend applications query:" << query.lastError().text();
                promise->addResult(applicationList);
                promise->finish();
                return;
            }
            qDebug() << "Query executed successfully";
            int count = 0;
			while (query.next()) {
                NewFriendApplicationlNS::user_application app;
                app.user_id = query.value(0).toString();
                app.username = query.value(1).toString();
                app.msg = query.value(2).toString();
                app.time = query.value(3).toDateTime();
                app.status = query.value(4).toInt();
                applicationList.append(app);
                count++;
                qDebug() << "Found application from user:" << app.user_id << app.user_id;
            }
            //qDebug() << "Total friend applications found:" << count;
            promise->addResult(applicationList);
            promise->finish();
        }
        catch (const std::exception& e) {
            qDebug() << "Exception in getApplicationList:" << e.what();
            promise->addResult(applicationList);
            promise->finish();
        }
        catch (...) {
            qDebug() << "Unknown exception in getApplicationList";
            promise->addResult(applicationList);
            promise->finish();
        }
        });
	return promise->future();
}

void HistoryDB_Mgr::getApplicationListAsync()
{
    //qDebug() << "Starting getApplicationListAsync...";
    auto future = this->getApplicationList();
    // 先设置 future，再连接信号
	// 断开之前的连接，避免重复连接
    disconnect(&this->friend_application_future_watcher, &QFutureWatcher<QList<NewFriendApplicationlNS::user_application>>::finished, nullptr, nullptr);
    this->friend_application_future_watcher.setFuture(future);
    connect(&this->friend_application_future_watcher, &QFutureWatcher<QList<NewFriendApplicationlNS::user_application>>::finished, [this]() {
        qDebug() << "Friend application list future watcher finished signal received";
        try {
            if (this->friend_application_future_watcher.isFinished()) {
                auto result = this->friend_application_future_watcher.result();
                qDebug() << "Friend application list loaded, count:" << result.size();
                // 发射信号通知UI更新
                emit sig_applicationListReady(result);
            }
            else {
                qDebug() << "Friend application list future is not finished yet";
            }
        }
        catch (const std::exception& e) {
            qDebug() << "Error getting friend application list result:" << e.what();
        }
        catch (...) {
            qDebug() << "Unknown error in friend application list future watcher";
        }
        });
}

bool HistoryDB_Mgr::storeApplicationList(const QList<NewFriendApplicationlNS::user_application>& application_list)
{
	//存储好友申请列表
	qDebug() << "Storing friend application list, count:" << application_list.size();
    std::lock_guard<std::mutex> lock(this->db_mutex);
    if (!this->chat_history_databases.isOpen()) {
        qDebug() << "Database is not open. Cannot store friend application list.";
        return false;
    }
    QSqlQuery query(this->chat_history_databases);
    if (!this->chat_history_databases.transaction()) {
        qDebug() << "Failed to start transaction:" << this->chat_history_databases.lastError().text();
        return false;
    }
    bool success = true;
    for (const auto& app : application_list) {
        query.prepare(R"(
            INSERT INTO friend_applications (user_id, user_name, msg, time, status)
            VALUES (:userId, :userName, :msg, :time, :status)
            ON CONFLICT(user_id) DO UPDATE SET
                user_id=excluded.user_id,
                user_name=excluded.user_name,
                msg=excluded.msg,
                time=excluded.time,
                status=excluded.status
        )");
        query.bindValue(":userId", app.user_id);
        query.bindValue(":userName", app.username);
        query.bindValue(":msg", app.msg);
        query.bindValue(":time", app.time.toString(Qt::ISODate));
        query.bindValue(":status", app.status);
        if (!query.exec()) {
            qDebug() << "Failed to insert friend application:" << query.lastError().text()
                << "SQL:" << query.lastQuery()
                << "userId:" << app.user_id;
            success = false;
            break;
        }
		emit the_user_icon_mgr::getInstance()->sig_the_update_the_user_icon(app.user_id, app.user_id, app.avatar);//更新头像
    }
    if (success) {
        if (!this->chat_history_databases.commit()) {
            qDebug() << "Failed to commit transaction:" << this->chat_history_databases.lastError().text();
            return false;
        }
    }
    else {
        this->chat_history_databases.rollback();
        return false;
    }
	return true;
}


void HistoryDB_Mgr::slot_storeApplicationList(const QList<NewFriendApplicationlNS::user_application>& application_list)
{
   if ( this->storeApplicationList(application_list))
	   qDebug() << "storeApplicationList done!";
   else
	   qDebug() << "storeApplicationList failed!";
}
