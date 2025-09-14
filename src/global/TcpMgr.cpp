#include "TcpMgr.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <qabstractsocket.h>

#include "gif_player.h"

TcpMgr::TcpMgr() :port(0), the_message_is_all(false), host(""),
                  message_length(0),message_id(0)
{
	connect(&socket, &QTcpSocket::connected, [this]()
		{
			qDebug() << "TCP connection successful!";
			qDebug() << "Connected to:" << this->socket.peerAddress().toString() << ":" << this->socket.peerPort();
			emit gif_player::getInstance()->sig_stop_display();
			emit sig_con_success(true);
		});
		
	connect(&socket, &QTcpSocket::disconnected, [this]()
		{
			qDebug() << "TCP connection disconnected";
		});
		
	connect(&socket, &QTcpSocket::readyRead, [this]()
		{
			//当有数据从tcp传来时
			this->buffer.append(this->socket.readAll());
			QDataStream output(&this->buffer, QIODevice::ReadOnly);
			output.setVersion(QDataStream::Qt_6_0);
			forever//一直解析数据直到无法解析
			{
				if (!this->the_message_is_all)
				{
					if (buffer.size()<static_cast<int>(sizeof(quint16))*2)//当缓冲区没有接收到一个完整的消息头的时候
						return;
					
					output >> this->message_id >> this->message_length;//将消息头写入
					this->buffer=this->buffer.mid(sizeof(quint16) * 2);//删除掉消息头 - 修复这里，应该从开头删除
					qDebug() << "Message id:" << this->message_id << "message length:" << this->message_length;
				}
				if (this->buffer.size() < message_length)
				{
					this->the_message_is_all = true;//此时消息头已近解析出来但是消息不够，将the_message_is_all设置为true这样下次触发就不会将消息体解析为消息头
					return;
				}
				this->the_message_is_all = false;//此时下一次重新开始处理消息头
				QByteArray message = this->buffer.mid(0, this->message_length);
				qDebug() << "message is received:" << message;
				this->buffer = this->buffer.mid(message_length);
				this->handler_msg(static_cast<Message_id>(message_id), message_length, message);
			}

		});

	connect(&this->socket, &QTcpSocket::errorOccurred, [this](QAbstractSocket::SocketError error)
		{
			qDebug() << "TCP Socket Error:" << error << "Error string:" << this->socket.errorString();
			switch (error)
			{
			case QAbstractSocket::HostNotFoundError:
				qDebug() << "无法连接到主机！";
				emit sig_con_success(false);
				break;
			case QAbstractSocket::SocketTimeoutError:
				qDebug() << "socket 超时！";
				emit sig_con_success(false);
				break;
			case QAbstractSocket::ConnectionRefusedError:
				qDebug() << "连接被拒绝！";
				emit sig_con_success(false);
				break;
			default:
				qDebug() << "其他网络错误！";
				emit sig_con_success(false);
				break;
			}
		});
		
	// 初始化处理器
	init_handler();
}


void TcpMgr::init_handler()
{
	this->handlers.insert(ID_CHAT_LOGIN_RSP, [this](Message_id id, int len, QByteArray data)
		{
			QString message_str(data);
			auto json_doc = QJsonDocument::fromJson(data);
			if (json_doc.isObject())
			{
				auto json_obj = json_doc.object();
				auto uid = json_obj["uid"].toInt();
				auto token = json_obj["token"].toString();
				if (!json_obj.contains("error"))
				{
					int err = ErrorCodes::ERR_JOSN;
					qDebug() << "Login Failed, err is Json Parse Err" << err;
					emit sig_login_failed(err);
					return;
				}
				auto error = json_obj["error"].toInt();
				if (!(error==ErrorCodes::SUCCESS))
				{
					qDebug() << "鉴权失败！Error code:" << error;
					emit sig_login_failed(error);
					return;
				}
				qDebug() << "TCP Login successful! UID:" << uid;
				emit gif_player::getInstance()->sig_stop_display();
				emit this->sig_swtich_chat_dialog();
			}
			else
			{
				qDebug() << "Invalid JSON received from server";
				emit sig_login_failed(ErrorCodes::ERR_JOSN);
			}
		});
}


void TcpMgr::handler_msg(Message_id id, int len, QByteArray data)
{
	qDebug() << "Handling message with ID:" << static_cast<int>(id) << "Length:" << len;
	
	auto func = this->find_handler(static_cast<qint16>(id));
	if (func.has_value())
	{
		func.value()(id, len, data);
	}
	else
	{
		qDebug() << "No handler found for message ID:" << static_cast<int>(id);
	}
}


void TcpMgr::send_data(Message_id message_id_id, QString const& message)
{
	// Check if socket is connected before sending data
	if (this->socket.state() != QAbstractSocket::ConnectedState) {
		qDebug() << "Cannot send data: socket is not connected. Current state:" << this->socket.state();
		return;
	}
	
	qint16 message_id = static_cast<qint16>(message_id_id);
	qint16 message_length = static_cast<qint16>(message.toUtf8().size()); // 修复：使用UTF8字节长度而不是字符串长度
	QByteArray data = message.toUtf8();
	QByteArray send_data;
	QDataStream out(&send_data, QIODevice::WriteOnly);
	out.setByteOrder(QDataStream::BigEndian);
	out << message_id << message_length;
	send_data.append(data);//将消息体追加
	
	qint64 bytesWritten = this->socket.write(send_data);
	if (bytesWritten == -1) {
		qDebug() << "Failed to write data to socket:" << this->socket.errorString();
	} else {
		qDebug() << "Successfully sent" << bytesWritten << "bytes. Message ID:" << message_id << "Length:" << message_length;
	}
}


void TcpMgr::connect_to_server(server_info server)
{
	// Check if socket is already connected or connecting
	if (this->socket.state() == QAbstractSocket::ConnectedState) {
		qDebug() << "Socket is already connected to" << this->socket.peerAddress().toString() << ":" << this->socket.peerPort();
		emit gif_player::getInstance()->sig_stop_display();
		return;
	}
	
	if (this->socket.state() == QAbstractSocket::ConnectingState || 
		this->socket.state() == QAbstractSocket::HostLookupState) {
		qDebug() << "Socket is already connecting, disconnecting first...";
		this->socket.disconnectFromHost();
		// Wait for disconnection before connecting
		if (this->socket.state() != QAbstractSocket::UnconnectedState) {
			this->socket.waitForDisconnected(3000); // Wait up to 3 seconds
		}
	}
	
	this->host = server.host;
	this->port = server.port;
	qDebug() << "正在连接到服务器！";
	qDebug() << "Host:" << this->host << "Port:" << this->port;
	qDebug() << "Socket state before connecting:" << this->socket.state();
	this->socket.connectToHost(this->host, this->port);
}

void TcpMgr::disconnect_from_server()
{
	if (this->socket.state() != QAbstractSocket::UnconnectedState) {
		qDebug() << "Disconnecting from server...";
		this->socket.disconnectFromHost();
		if (this->socket.state() != QAbstractSocket::UnconnectedState) {
			this->socket.waitForDisconnected(3000);
		}
		qDebug() << "Disconnected. Socket state:" << this->socket.state();
	}
}

std::optional<std::function<void(Message_id id, int len, QByteArray data)>> TcpMgr::find_handler(qint16 message_id)
{
	auto pos = this->handlers.find(static_cast<Message_id>(message_id));
	if (pos == this->handlers.end())
	{
		qDebug() << "没有相关的处理函数！Message ID:" << message_id;
		return std::nullopt;
	}
	return handlers[static_cast<Message_id>(message_id)];
}
