#include "mistakekmgr.h"
#include <QDebug>


MistakeMgr::MistakeMgr(QObject*parent):QObject(parent)
{
	this->mistakes.insert(ErrorCodes::UserNameIsNull, QString("用户名不能为空！"));
	this->mistakes.insert(ErrorCodes::UserPasswdError, QString("密码错误！"));
	this->mistakes.insert(ErrorCodes::EmailStlyeNotRight, QString("邮箱格式不正确！"));
	this->mistakes.insert(ErrorCodes::UserPasswdIsNull, QString("密码不能为空！"));
}

void MistakeMgr::add_mistake(std::pair<ErrorCodes,QString>mistake)
{
    this->mistakes.insert(mistake.first,mistake.second);
}

void MistakeMgr::delete_mistake(const ErrorCodes& key)
{
    auto pos=this->mistakes.find(key);
    if(pos==this->mistakes.end())
    {
        qDebug()<<"can not find the key!";

    }
    else{
        this->mistakes.remove(key);
        if(this->mistakes.count()==0)
           emit this->the_mistake_is_null();
    }
}


size_t MistakeMgr::get_mistake_nums()
{
    return this->mistakes.count();
}
