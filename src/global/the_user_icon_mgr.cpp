#include "the_user_icon_mgr.h"
the_user_icon_mgr::the_user_icon_mgr()
{
	QString App_Path = QApplication::applicationDirPath();//获取应用程序路径
	QString db_path = App_Path + "/user_icon_db/user_icon.db";
	if (!QDir(App_Path + "/user_icon_db").exists()) {// 检查目录是否存在
		QDir().mkdir(App_Path + "/user_icon_db");
		if (!QFile::exists(App_Path))
		{
			QFile user_icon_db_file;
			user_icon_db_file.setFileName(db_path);
			if (!user_icon_db_file.open(QIODevice::WriteOnly)) {
				qDebug() << "Failed to create user icon database file.";
				return;
			}
			user_icon_db_file.close();
		}
		this->db = QSqlDatabase::addDatabase("QSQLITE");
		this->db.setDatabaseName(db_path);
		this->db.open();
		if (!this->db.isOpen())
		{
			qDebug() << "Failed to open database:" << this->db.lastError().text();
			return;
		}
		else
		{
			this->init_db(); // 初始化数据库
			qDebug() << "Database created and opened successfully.";
		}
	}
	else//数据库文件存在
	{
		this->db = QSqlDatabase::addDatabase("QSQLITE");
		this->db.setDatabaseName(db_path);
		this->db.open();
		this->init_db(); // 初始化数据库
		if (!this->db.isOpen())
		{
			qDebug() << "Failed to open database:" << this->db.lastError().text();
			return;
		}
	}
	connect(this, &the_user_icon_mgr::sig_the_update_the_user_icon, this, &the_user_icon_mgr::set_user_icon);
}

the_user_icon_mgr::~the_user_icon_mgr()
{
	if (this->db.isOpen()) {
		this->db.close(); // 关闭数据库连接
		qDebug() << "Database connection closed.";
	}
	else {
		qDebug() << "Database was not open.";
	}
}

void the_user_icon_mgr::init_db()
{
	QByteArray byteArray;
	if (this->user_icon.load(":/res/default_user_icon.png"))//先从资源文件中加载默认用户头像
		{
		qDebug() << "Default user icon loaded successfully.";
		QDataStream stream(&byteArray, QIODevice::WriteOnly);
		stream << this->user_icon; // 将QPixmap转换为字节数组;
	} else {
		qDebug() << "Failed to load default user icon.";
		return;
	}
	if (!this->db.isOpen()) {
		qDebug() << "Database is not open. Cannot initialize.";
		return;
	}
	this->query = QSqlQuery(this->db);
	this->query.exec("CREATE TABLE IF NOT EXISTS user_icons ("
		"user_id TEXT PRIMARY KEY, "
		"user_email TEXT, "
		"icon BLOB)");// 创建用户头像表
	this->query.prepare("SELECT user_id FROM user_icons WHERE user_id=:id OR user_email=:email");// 检查默认用户头像是否已经存在
	this->query.bindValue(":id", "default_user_icon");// 默认用户头像ID
	this->query.bindValue(":email", "default_user_email");// 默认用户头像邮箱
	if (!this->query.exec()) {
		qDebug() << "Failed to execute query:" << this->query.lastError().text();
		return;
	}
	if (this->query.next()) {
		qDebug() << "Default user icon already exists in the database.";
		return; // 默认用户头像已经存在
	}
	this->query.prepare("INSERT INTO user_icons(user_id,user_email,icon)" " VALUES(:id, :email, :pix);");

	this->query.bindValue(":id", "default_user_icon");// 默认用户头像ID
	this->query.bindValue(":email", "default_user_email");// 默认用户头像邮箱
	this->query.bindValue(":pix", byteArray);// 默认用户头像数据
	if (!this->query.exec()) {
		qDebug() << "Failed to insert default user icon:" << this->query.lastError().text();
	} else {
		qDebug() << "Default user icon inserted successfully.";
	}

}

QPixmap the_user_icon_mgr::get_user_icon(QString user_id, QString user_email)
{
	this->query.prepare("SELECT icon FROM user_icons WHERE user_id=:id OR user_email=:email");
	this->query.bindValue(":id", user_id);
	this->query.bindValue(":email", user_email);
	if (!this->query.exec()) {
		qDebug() << "Failed to execute query:" << this->query.lastError().text();
		return { ":/res/default_user_icon.png" }; // 返回空的QPixmap
	}
	else
	{
		if (this->query.next())// 如果查询结果有下一行及存在该用户头像
		{
			auto result = this->query.value(0);
			if (result.isNull()) {
				qDebug() << "No icon found for user_id:" << user_id;
				return  QPixmap{ ":/res/default_user_icon.png" }; // 返回空的QPixmap
			}
				QByteArray byteArray = result.toByteArray();
				QDataStream stream(&byteArray, QIODevice::ReadOnly);
				QPixmap user_icon;
				stream >> user_icon; // 从字节数组中读取QPixmap
				if (user_icon.isNull()) {
					qDebug() << "Failed to load user icon for user_id:" << user_id;
					return {}; // 返回空的QPixmap
				}
				else {
					return user_icon; // 返回用户头像
				}
		}
		else// 如果查询结果没有下一行及不存在该用户头像
		{
			qDebug() << "No icon found for user_id:" << user_id;
			return { ":/res/default_user_icon.png" };
		}
	}
}

void the_user_icon_mgr::set_user_icon(const QString& user_id, const QString& user_email, const QPixmap& user_icon)// 设置用户头像同时更新本地的头像数据库
{
	QByteArray byteArray;
	QDataStream stream(&byteArray, QIODevice::WriteOnly);
	stream << user_icon; // 将QPixmap转换为字节数组

	std::lock_guard<std::mutex> lock(icon_mutex); // 加锁以确保线程安全
	this->query.prepare("REPLACE INTO user_icons (user_id, user_email, icon) VALUES (:id, :email, :pix)");
	this->query.bindValue(":id", user_id);
	this->query.bindValue(":email", user_email);
	this->query.bindValue(":pix", byteArray);
	if (!this->query.exec()) {
		qDebug() << "Failed to insert/update user icon:" << this->query.lastError().text();
	} else {
		qDebug() << "User icon inserted/updated successfully.";
		emit sig_notify_the_icon_frame(user_icon);
	}
}
