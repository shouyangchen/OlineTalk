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
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include <QString>
#include <QDir>

//新增异步获取用户头像功能
extern  QString user_icon_database_path;
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
	QFutureWatcher<std::pair<QPixmap, QString>>future_watcher;// 监视异步任务
	QList<std::pair<QPixmap,QString>> icon_list;// 存储异步获取的用户头像
	void init_db(); // 初始化数据库
	QFuture<std::pair<QPixmap,QString>> loading_user_icon(const QList<QString> user_id);
	signals:
		void user_icon_changed(QString &user_id,QPixmap&user_icon); // 用户头像改变时发出的信号
		void sig_the_update_the_user_icon(QString& user_id,QString &email, QPixmap& user_icon);
		void sig_notify_the_icon_frame(const QPixmap& user_icon);//当发生变化后通知绘制界面
		void sig_notify_the_user_icon_loading_done(const QList<std::pair<QPixmap,QString>>& icon_list);//异步获取用户头像完成后发出该信号
public:
	virtual ~the_user_icon_mgr();
	the_user_icon_mgr(const the_user_icon_mgr&) = delete;
	the_user_icon_mgr& operator=(const the_user_icon_mgr&) = delete;
	QPixmap get_user_icon(QString user_id,QString user_email);
	void get_user_icon_async(const QList<QString> user_list);// 异步获取用户头像

public slots:
	void set_user_icon(const QString& user_id, const QString& user_email, const QPixmap& user_icon);
	void slot_get_user_icon_async(const QList<QString> user_list);
};
#endif // THE_USER_ICON_MGR_H

