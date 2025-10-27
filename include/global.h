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
extern bool is_first_login;//是否是第一次登录是则需要下载用户头像等信息和加载登录用户的联系人列表
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
enum Message_id:quint32
{
    LOGIN_MESSAGE = 3001,
    SEND_MESSAGE_HANDLER = 3002,
    HEARTBEAT_PACKET = 3003,
    ID_CHAT_LOGIN_RSP = 3004,//用户登录
    SEARCH_USER = 3005,//搜索用户
    FILE_CLIENT_VERIFY = 3006,//文件操作进程验证用户连接
    TRANS_MESSAGE_TO_OTHER_SERVER = 3007,//转发消息到其他服务器
    ADD_FRIEND_REQUEST = 3008,//添加好友请求
    ADD_FRIEND_RESPONSE = 3009,//添加好友响应
    GET_ADD_FRIEND_REQUESTS = 3010,//获取好友请求列表
    GET_FRIEND_LIST = 3011,//获取好友列表
	DELETE_FRIEND = 3012,//删除好友
    UPDATE_FRIEND_INFO = 3013,//更新好友信息
};

enum  ErrorCodes:std::uint16_t{
    SUCCESS=0, //成功
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
	QPixmap pixmap;//如果为图片或文件则存储缩略图
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

namespace connectUserList
{
    struct user_info
    {
        QString username;
        QString user_id;
        QPixmap avatar;

        // 默认构造函数
        user_info() = default;

        // 方便构造函数
        user_info(const QString& id, const QString& name, const QPixmap& pic = QPixmap())
            : username(name), user_id(id), avatar(pic) {
        }
    };
}



namespace NewFriendApplicationlNS
{
    struct user_application
    {
        QString username;
        QPixmap avatar;
        QString msg;
        QDateTime time;
        QString user_id;
		int status; // 0:未处理, 1:已同意, 2:已拒绝
        user_application() = default;
        user_application(const QString& name, const QPixmap& pic, const QString& m, const QDateTime& t,const QString &id, int s = 0)
            : username(name), avatar(pic), msg(m), time(t),user_id(id), status(s) {
		}
    };
}

#endif //GLOBAL_H
