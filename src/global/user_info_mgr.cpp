#include "user_info_mgr.h"

user_info_mgr::user_info_mgr(QPixmap icon, QString name, QString sex, std::uint64_t id)
:user_iocn(std::move(icon)),user_name(std::move(name)),user_sex(std::move(sex)),user_id(id)
{
	
}

QPixmap& user_info_mgr::get_user_icon()
{
	return this->user_iocn;
}

const std::uint64_t& user_info_mgr::get_user_id()
{
	return this->user_id;
}




QString& user_info_mgr::get_user_name()
{
	return this->user_name;
}


QString& user_info_mgr::get_user_sex()
{
	return this->user_sex;
}

void user_info_mgr::set_user_icon(QPixmap icon)
{
	this->user_iocn = icon;
}


void user_info_mgr::set_user_name(QString name)
{
	this->user_name = name;
}


void user_info_mgr::set_user_sex(QString sex)
{
	this->user_sex = sex;
}


void user_info_mgr::set_user_uid(std::uint64_t uid)
{
	this->user_id = uid;
}
