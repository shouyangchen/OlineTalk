#ifndef THE_USER_ICON_MGR_H
#define THE_USER_ICON_MGR_H
#include "singleton.h"
#include <QObject>
#include <QApplication>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSqlRecord>
#include <QtSql/QSqlError>
#include <QFile>
#include <QMap>
#include <QString>
#include <QDir>

class the_user_icon_mgr :public QObject,public singleton<the_user_icon_mgr>
{
	Q_OBJECT
private:
	friend class singleton<the_user_icon_mgr>;
	the_user_icon_mgr();
	QSqlDatabase db;
	QSqlQuery query;
	QFile icon_file;
	std::mutex icon_mutex; // 互斥锁用于线程安全
	QPixmap user_icon;
	void init_db(); // 初始化数据库
	signals:
		void user_icon_changed(QString &user_id,QPixmap&user_icon); // 用户头像改变时发出的信号
		void sig_the_update_the_user_icon(QString& user_id,QString &email, QPixmap& user_icon);
		void sig_notify_the_icon_frame(const QPixmap& user_icon);//当发生变化后通知绘制界面
public:
	virtual ~the_user_icon_mgr();
	the_user_icon_mgr(const the_user_icon_mgr&) = delete;
	the_user_icon_mgr& operator=(const the_user_icon_mgr&) = delete;
	QPixmap get_user_icon(QString user_id,QString user_email);

public slots:
	void set_user_icon(const QString& user_id, const QString& user_email, const QPixmap& user_icon);
};
#endif // THE_USER_ICON_MGR_H

