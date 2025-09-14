#ifndef TCPMGR
#define TCPMGR
#include "singleton.h"
#include <QJsonArray>
#include <QJsonObject>
#include "global.h"
#include <QAbstractSocket>
#include <QTcpSocket>
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
    quint16 message_length;
    quint16 message_id;
	bool the_message_is_all;
    QMap<Message_id, std::function<void(Message_id id, int len, QByteArray data)>> handlers;
    void init_handler();//初始化handler
    void handler_msg(Message_id id, int len, QByteArray data);//分发处理函数
    std::optional<std::function<void(Message_id id, int len, QByteArray data)>> find_handler(qint16 message_id);

public:
    // Add method to check connection state
    bool isConnected() const { return socket.state() == QAbstractSocket::ConnectedState; }
    void disconnect_from_server();
    
signals:
    void sig_con_success(bool success);
    void sig_send_data(Message_id id, QByteArray data);
    void sig_connect_server(server_info);
    void sig_swtich_chat_dialog();
    void sig_load_apply_list(QJsonArray json_array);
    void sig_login_failed(int);
public slots:
    void send_data(Message_id message_id_id, QString const & message);
    void connect_to_server(server_info server);
};
#endif

