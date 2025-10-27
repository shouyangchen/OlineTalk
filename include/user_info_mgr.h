#ifndef USER_INFO_H
#define USER_INFO_H
#include <type_traits>
#include <QPixmap>
#include "singleton.h"


class user_info_mgr:public singleton<user_info_mgr>,public std::enable_shared_from_this<user_info_mgr>
{
private:
	friend class singleton<user_info_mgr>;
	user_info_mgr(QPixmap icon,QString name,QString sex,std::uint64_t id);
	QPixmap user_iocn;
	QString user_name;
	QString user_sex;
	std::uint64_t user_id;
public:
	user_info_mgr(user_info_mgr&) = delete;
	user_info_mgr(user_info_mgr&&) = delete;
	user_info_mgr& operator=(user_info_mgr&) = delete;
	QPixmap& get_user_icon();
	QString& get_user_name();
	QString& get_user_sex();
	const std::uint64_t& get_user_id();
	void set_user_icon(QPixmap icon);
	void set_user_name(QString name);
	void set_user_sex(QString sex);
	void set_user_uid(std::uint64_t uid);

};


class SearchInfo {
public:
	SearchInfo() = default;
	SearchInfo(QString uid, QString name, QString nick, QString desc, int sex, QString icon);
	QString uid;//用户ID
	QString name;//用户名
	QString nick;//昵称
	QString desc;//个性签名
	int sex;//性别
	QString icon;//头像URL
};


#endif

