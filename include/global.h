//
// Created by chenshouyang on 25-6-23.
//

#ifndef GLOBAL_H
#define GLOBAL_H
#include <functional>
#include <QSettings>
#include <QDateTime>
#include <QWidget>
#include <QStyle>
#include <QRegularExpression>
#include <QCryptographicHash>
#include <QString>
#include <QMap>

extern QString gate_url_prefix;
extern std::function<void(QWidget*)> repolish_widget_func;//刷新QSS的函数

// HTTP请求ID枚举 (1000-1999)
enum  ReqId:std::uint16_t{
    ID_GET_VARIFY_CODE=1001,  //获取验证码
    ID_REG_USER=1002,//注册用户
	ID_USER_LOGIN = 1003, //用户登录
	ID_GET_VARIFY_CODE_FORGET_PASSWD = 1004, //获取忘记密码的验证码
	ID_SUBMIT_FORGET_PASSWD = 1005, //忘记密码
    ID_GET_USER_ICON=1006
};

// TCP消息ID枚举 (3000-3999)
enum Message_id:qint16
{
	LOGIN_MESSAGE=3001,              // 登录消息
    SEND_MESSAGE_HANDLER = 3002,     // 发送消息处理
    HEARTBEAT_PACKET = 3003,         // 心跳包
    ID_CHAT_LOGIN_RSP=3004,           // 用户登录响应
	SEARCH_USER             // 搜索用户
};

enum  ErrorCodes:std::uint16_t{
    SUCCESS=0, //成功}
    ERR_JOSN=1, //json错误
    ERR_NETWORK=2, //网络错误
    VarifyError = 1004,// 验证码错误
    EmailIsRegister = 1008,//邮箱已注册
	UserNotExist = 1009,//用户不存在
    UserPasswdError = 1010,//用户密码错误
	EmailStlyeNotRight,//邮箱格式不正确
	UserNameIsNull,//用户名为空
	UserPasswdIsNull,//密码为空
    TokenError = 1014,
    USER_ICON_NULL,
      /**
	 * UserPasswdError 用户密码错误
	 * EmailStlyeNotRight 邮箱格式不正确
	 * 
     */
};

enum  Modules:std::uint16_t{
    MODULE_LOGIN=1, //登录模块}
    MODULE_REGISTER=2, //注册模块
    MODULE_USER=3, //用户模块
    MODULE_CHAT=4, //聊天模块
    MODULE_FRIENDS=5, //好友模块
    MODULE_GROUP=6, //群组模块
    MODULE_FILE_TRANSFER=7, //文件传输模块
    MODULE_NOTIFICATION=8, //通知模块
	MODULE_FORGET_PASSWD = 9,//忘记密码模块
	MODULE_USER_SEARCH = 10//用户搜索模块
};

enum the_gif_order:std::uint16_t
{
	THE_LOADING_GIF=2000,
    THE_LOADING_DONE=2001,
};


enum ListItemType
{
	CHAT_USER_ITEM,//聊天用户
    CONNECT_USER_ITEM,//联系人用户
    ADD_USER_ITEM,//添加用户
    SERACH_USER_ITEM,//搜索到的用户
    ADD_USER_TIP_ITEM,//提示添加用户
    INVALID_ITEM,//不可点击的条目
    GROUP_TIP_ITEM,//分组提示条目

};

extern QMap<QString,QString> has_mistalk;

struct server_info {//服务器信息结构体
	QString host;//chat server 的ip
	quint16 port;//chat server 的端口
};

enum ChatRole
{
    SELF,
    OTHER
};

//struct MsgInfo {
//    QString msgFlag;//"text,image,file"
//    QString content;//表示文件和图像的url,文本信息
//    QPixmap pixmap;//文件和图片的缩略图
//};


inline bool adjust_is_email(QString&text)
{
   
    QRegularExpression regex(R"([A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,})");// 正则表达式匹配邮箱格式
    return  regex.match(text).hasMatch();// 检查邮箱格式是否正确
}

struct ChatMessage
{
    QVariant message_context;
	QDateTime timestamp;
    QString message_type;
	QString sender;//消息发送者SELF or OTHER
    QPixmap pixmap;
};


class RecentChatListModel;

class the_connected_user_info
{
private:
    friend RecentChatListModel;
    QString user_id;
    QString user_name;
    QPixmap user_avatar;// 头像
    QString last_message;// 最后一条消息
    QDateTime last_message_time;// 最后消息时间
    int unread_message_count{ 0 };

public:
    the_connected_user_info() = default;
    the_connected_user_info(QString const& id, QString const& name, QPixmap const& avatar);
    const QString& get_user_id() const;
    const QString& get_user_name() const;
    QPixmap get_user_avatar() const;
    const QString& get_last_message() const;
    const QDateTime& get_last_message_time() const;
    int get_unread_message_count() const;
    void set_user_name(QString const& name);
    void set_user_avatar(QPixmap const& avatar);
    void set_last_message(QString const& message);
    void set_last_message_time(QDateTime const& time);
    void set_unread_message_count(int count);
    void set_user_id(QString const& uid);
    void increment_unread_message_count();
    void clear_unread_message_count();

};
#endif //GLOBAL_H
