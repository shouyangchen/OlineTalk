#ifndef TCPMGR
#define TCPMGR
#include "singleton.h"
#include <QJsonArray>
#include <QTimer>
#include <QJsonObject>
#include "global.h"
#include <QAbstractSocket>
#include <QTcpSocket>

#include "user_info_mgr.h"

class TcpMgr :public QObject,
              public singleton<TcpMgr>
{
    Q_OBJECT
private:
    friend class singleton<TcpMgr>;
    TcpMgr();
    QTcpSocket socket;
    quint16 port;
    QString host;
    QByteArray buffer;
    quint32 message_length;
    quint32 message_id;
	bool the_message_is_all;
    QTimer heartbeat_packet;
    QMap<Message_id, std::function<void(Message_id id,quint32 len, QByteArray data)>> handlers;
    void init_handler();//初始化handler
    void handler_msg(Message_id id, quint32 len, QByteArray data);//分发处理函数
    std::optional<std::function<void(Message_id id, quint32 len, QByteArray data)>> find_handler(qint32 message_id);
	void set_hearbeat_timer();//设置心跳包定时器

public:
    // Add method to check connection state
    bool isConnected() const { return socket.state() == QAbstractSocket::ConnectedState; }
    void disconnect_from_server();
    
signals:
    void sig_con_success(bool success);
    void sig_send_data(Message_id id, QByteArray data);
    void sig_connect_server(server_info);
    void sig_swtich_chat_dialog();
	void sig_load_apply_list(QJsonArray json_array);//加载好友申请列表
	void sig_login_failed(int);//登录失败
	void sig_user_serach(std::vector<std::shared_ptr<SearchInfo>>user_info);//搜索到用户
	void sig_new_message(ChatMessage message,QString from_uid);//有新的消息到达
	void sig_update_friend_list(QList<connectUserList::user_info> users_list);//更新好友列表
	void sig_new_friend_added(QString user_id, QString friend_name, QPixmap friend_avatar,QString application_message);//有一个新的好友添加
	void sig_friend_application_list(std::vector<NewFriendApplicationlNS::user_application> application_list);//好友申请列表
public slots:
    void send_data(Message_id message_id_id, QString const & message);
    void connect_to_server(server_info server);
};
#endif

