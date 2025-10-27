//
// Created by chenshouyang on 25-6-23.
//
#include "global.h"
std::function<void(QWidget*)> repolish_widget_func = [](QWidget*w)  {
    // Default implementation, can be overridden
   w->style()->unpolish(w);//卸载当前样式
   w->style()->polish(w); // 重新应用样式
};

QString gate_url_prefix = "";


//初始化全局变量
bool is_first_login = false;//是否是第一次登录是则需要下载用户头像等信息和加载登录用户的联系人列表

// ------------------ the_connected_user_info 实现 ------------------

the_connected_user_info::the_connected_user_info(QString const& id, QString const& name, QPixmap const& avatar)
	: user_id(id), user_name(name), user_avatar(avatar)
{
}

const QString& the_connected_user_info::get_user_id() const { return user_id; }
const QString& the_connected_user_info::get_user_name() const { return user_name; }
QPixmap the_connected_user_info::get_user_avatar() const { return user_avatar; }
const QString& the_connected_user_info::get_last_message() const { return last_message; }
const QDateTime& the_connected_user_info::get_last_message_time() const { return last_message_time; }
int the_connected_user_info::get_unread_message_count() const { return unread_message_count; }
void the_connected_user_info::set_user_name(QString const& name) { user_name = name; }
void the_connected_user_info::set_user_avatar(QPixmap const& avatar) { user_avatar = avatar; }
void the_connected_user_info::set_last_message(QString const& message) { last_message = message; }
void the_connected_user_info::set_last_message_time(QDateTime const& time) { last_message_time = time; }
void the_connected_user_info::set_unread_message_count(int count) { unread_message_count = count; }
void the_connected_user_info::increment_unread_message_count() { ++unread_message_count; }
void the_connected_user_info::clear_unread_message_count() { unread_message_count = 0; }
void the_connected_user_info::set_user_id(const QString& uid) { this->user_id = uid; }


