#include "TcpMgr.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <qabstractsocket.h>

#include "gif_player.h"
#include "HttpMgr.h"
#include "the_user_icon_mgr.h"

TcpMgr::TcpMgr() : port(0), host(""), message_length(0),
                   message_id(0), the_message_is_all(false)
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
		forever //一直解析数据直到无法解析
		{
			if (!this->the_message_is_all)
			{
				if (buffer.size() < static_cast<int>(sizeof(quint32)) * 2) //当缓冲区没有接收到一个完整的消息头的时候
					return;

				output >> this->message_id >> this->message_length; //将消息头写入
				this->buffer = this->buffer.mid(sizeof(quint32) * 2); //删除掉消息头 - 修复这里，应该从开头删除
				qDebug() << "Message id:" << this->message_id << "message length:" << this->message_length;
				qDebug() << "新收到的数据，消息ID:" << this->message_id << "消息长度：" << this->message_length;
			}
			if (this->buffer.size() < message_length)
			{
				this->the_message_is_all = true; //此时消息头已近解析出来但是消息不够，将the_message_is_all设置为true这样下次触发就不会将消息体解析为消息头
				return;
			}
			this->the_message_is_all = false; //此时下一次重新开始处理消息头
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
	// 设置心跳包定时器
	this->set_hearbeat_timer();
}


void TcpMgr::init_handler()
{
	this->handlers.insert(ID_CHAT_LOGIN_RSP, [this](Message_id id, quint32 len, QByteArray data)//登录响应
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
				int err = ERR_JOSN;
				qDebug() << "Login Failed, err is Json Parse Err" << err;
				emit sig_login_failed(err);
				return;
			}
			auto error = json_obj["error"].toInt();
			if (!(error == SUCCESS))
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
			emit sig_login_failed(ERR_JOSN);
		}
	});
	this->handlers.insert(SEARCH_USER, [this](Message_id id, quint32 len, QByteArray data)//处理搜索用户响应
	{
		QString message_str(data);
		auto json_doc = QJsonDocument::fromJson(data);
		if (json_doc.isObject())
		{
			auto json_obj = json_doc.object();
			auto error = json_obj["error"].toInt();
			if (error == SUCCESS && json_obj["search_results"].isArray()) //服务器返回成功
			{
				std::vector<std::shared_ptr<SearchInfo>> user_info_list;
				auto json_array = json_obj["search_results"].toArray();
				for (auto item : json_array)
				{
					if (item.isObject())
					{
						auto item_obj = item.toObject();
						auto user_info = std::make_shared<SearchInfo>();
						auto uid_json = item_obj["user_uid"];
						if (uid_json.isString())
						{
							user_info->uid = uid_json.toString();
						}
						else if (uid_json.isDouble())
						{
							user_info->uid = QString::number(uid_json.toInt());
						}
						user_info->name = QString::fromUtf8(item_obj["username"].toString().toUtf8());
						user_info->icon = item_obj["avatar"].toString();
						user_info->nick = QString::fromUtf8("NULL");
						user_info->sex = {};
						user_info->desc = QString::fromUtf8("NULL");
						user_info_list.push_back(user_info);
					}
					else
						return;
				}
				emit this->sig_user_serach(user_info_list);
			}
		}
		else
		{
			qDebug() << "Invalid JSON received from server for user search";
		}
	});

	this->handlers.insert(HEARTBEAT_PACKET, [this](Message_id id, int len, QByteArray data)
	{
		qDebug() << "Heartbeat packet received from server";
	});

	this->handlers.insert(GET_ADD_FRIEND_REQUESTS, [this](Message_id id, quint32 len, QByteArray data)//获取好友申请列表
	{
		QString message_str(data);
		auto json_doc = QJsonDocument::fromJson(data);
		if (json_doc.isObject())
		{
			auto json_obj = json_doc.object();
			auto error = json_obj["error"].toInt();
			if (error == SUCCESS && json_obj["friend_requests"].isArray()) //服务器返回成功
			{
				std::vector<NewFriendApplicationlNS::user_application> user_application_list;
				auto json_array = json_obj["friend_requests"].toArray();
				for (auto item:json_array)
				{
					if (item.isObject())
					{
						auto item_obj = item.toObject();
						auto user_id = QString::number(static_cast<std::uint64_t>(item_obj["applicant_uid"].toDouble()));
						auto user_name = QString::fromUtf8(item_obj["applicant_username"].toString().toUtf8());//申请人用户名
						auto application_message = QString::fromUtf8(item_obj["message"].toString().toUtf8());//申请消息
						auto application_time = QString::fromUtf8(item_obj["request_time"].toString().toUtf8());//时间字符串
						QDateTime application_datetime = QDateTime::fromString(application_time, "yyyy-MM-dd HH:mm:ss");
						user_application_list.emplace_back(user_name, QPixmap{},application_message, application_datetime,user_id);
					}
				}
				emit this->sig_friend_application_list(user_application_list);//发送信号加载好友申请列表
			}
		}
		else
		{
			qDebug() << "Invalid JSON received from server for friend requests";
		}
		});



	this->handlers.insert(SEND_MESSAGE_HANDLER, [this](Message_id id, quint32 len, QByteArray data)//处理新接受到的消息
	{
		QString message_str(data);
		auto json_doc = QJsonDocument::fromJson(data);
		if (json_doc.isObject())
		{
			auto json_obj = json_doc.object();
			if (json_obj["error"].toInt()!=ErrorCodes::SUCCESS)
			{
				return;
			}
			auto send_uid = static_cast<std::uint64_t>(json_obj["send_uid"].toDouble());
			//拿到发送者的uid
			auto message_content = QString::fromUtf8(json_obj["message"].toString().toUtf8());//进行utf8解码
			auto timestamp = json_obj["timestamp"].toString();
			auto message_type=json_obj["message_type"].toString();
			QDateTime message_datetime = QDateTime::fromString(timestamp, "yyyy-MM-dd HH:mm:ss");
			ChatMessage chat_message;
			chat_message.sender = " OTHER";
			chat_message.message_type = message_type;
			chat_message.timestamp = message_datetime;
			chat_message.message_context = message_content;//消息内容
			emit this->sig_new_message(chat_message, QString::number(send_uid));//发送信号通知有新的消息到达

		}
		else
		{
			qDebug() << "Invalid JSON received from server for new message";
		}
		});


	this->handlers.insert(ADD_FRIEND_REQUEST, [this](Message_id id, quint32 len, QByteArray data)//处理添加好友请求的响应
	{
		QString message_str(data);
		auto json_doc = QJsonDocument::fromJson(data);
		if (json_doc.isObject())
		{
			auto json_obj = json_doc.object();
			auto error = json_obj["error"].toInt();
			if (error == SUCCESS) //服务器返回成功
			{
				auto new_friend_uid = QString::number(static_cast<std::uint64_t>(json_obj["friend_uid"].toDouble()));
				auto new_friend_name = json_obj["friend_username"].toString();
				auto new_friend_avatar_url = json_obj["friend_avatar"].toString();
				auto application_message = json_obj["application_message"].toString();
				//异步获取好友头像
				the_user_icon_mgr::getInstance()->get_user_icon_async({ new_friend_uid });
				QPixmap friend_avatar;
				emit this->sig_new_friend_added(new_friend_uid, new_friend_name, friend_avatar,application_message);//发送信号通知有新的好友添加
			}
		}
		else
		{
			qDebug() << "Invalid JSON received from server for add friend request";
		}
		});

	this->handlers.insert(DELETE_FRIEND, [this](Message_id id, quint32 len, QByteArray data)//处理删除好友的响应
	{
		
	});

	this->handlers.insert(UPDATE_FRIEND_INFO, [this](Message_id id, quint32 len, QByteArray data)//处理更新好友信息的响应
		{

		});
}

void TcpMgr::handler_msg(Message_id id, quint32 len, QByteArray data)
{
	qDebug() << "Handling message with ID:" << id << "Length:" << len;

	auto func = this->find_handler(id);
	if (func.has_value())
	{
		func.value()(id, len, data);
	}
	else
	{
		qDebug() << "No handler found for message ID:" << id;
	}
}


void TcpMgr::send_data(Message_id message_id_id, const QString& message)
{
	// Check if socket is connected before sending data
	if (this->socket.state() != QAbstractSocket::ConnectedState)
	{
		qDebug() << "Cannot send data: socket is not connected. Current state:" << this->socket.state();
		return;
	}

	qint32 message_id = message_id_id;
	qint32 message_length = static_cast<qint32>(message.toUtf8().size()); // 修复：使用UTF8字节长度而不是字符串长度
	QByteArray data = message.toUtf8();
	QByteArray send_data;
	QDataStream out(&send_data, QIODevice::WriteOnly);
	out.setByteOrder(QDataStream::BigEndian);
	out << message_id << message_length;
	send_data.append(data); //将消息体追加

	qint64 bytesWritten = this->socket.write(send_data);
	if (bytesWritten == -1)
	{
		qDebug() << "Failed to write data to socket:" << this->socket.errorString();
	}
	else
	{
		qDebug() << "Successfully sent" << bytesWritten << "bytes. Message ID:" << message_id << "Length:" <<
			message_length;
	}
}


void TcpMgr::connect_to_server(server_info server)
{
	// Check if socket is already connected or connecting
	if (this->socket.state() == QAbstractSocket::ConnectedState)
	{
		qDebug() << "Socket is already connected to" << this->socket.peerAddress().toString() << ":" << this->socket.
			peerPort();
		emit gif_player::getInstance()->sig_stop_display();
		return;
	}

	if (this->socket.state() == QAbstractSocket::ConnectingState ||
		this->socket.state() == QAbstractSocket::HostLookupState)
	{
		qDebug() << "Socket is already connecting, disconnecting first...";
		this->socket.disconnectFromHost();
		// Wait for disconnection before connecting
		if (this->socket.state() != QAbstractSocket::UnconnectedState)
		{
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
	if (this->socket.state() != QAbstractSocket::UnconnectedState)
	{
		qDebug() << "Disconnecting from server...";
		this->socket.disconnectFromHost();
		if (this->socket.state() != QAbstractSocket::UnconnectedState)
		{
			this->socket.waitForDisconnected(3000);
		}
		qDebug() << "Disconnected. Socket state:" << this->socket.state();
	}
}

std::optional<std::function<void(Message_id id, quint32 len, QByteArray data)>> TcpMgr::find_handler(qint32 message_id)
{
	auto pos = this->handlers.find(static_cast<Message_id>(message_id));
	if (pos == this->handlers.end())
	{
		qDebug() << "没有相关的处理函数！Message ID:" << message_id;
		return std::nullopt;
	}
	return handlers[static_cast<Message_id>(message_id)];
}


void TcpMgr::set_hearbeat_timer()
{
	// 设置心跳包定时器
	this->heartbeat_packet.setInterval(60000); // 每60秒发送一次心跳包
	this->heartbeat_packet.connect(&this->heartbeat_packet, &QTimer::timeout, [this]()
	{
		// 如果未连接，不发送心跳
		if (this->socket.state() != QAbstractSocket::ConnectedState)
		{
			qDebug() << "Heartbeat skipped: socket not connected";
			return;
		}

		QJsonObject heartbeat;
		auto now = QDateTime::currentDateTime();
		heartbeat["type"] = "heartbeat";
		heartbeat["timestamp"] = QDateTime::currentDateTime().toSecsSinceEpoch();
		heartbeat["formatted_time"] = now.toString("yyyy-MM-dd HH:mm:ss");
		heartbeat["server_uptime"] = "online";
		QJsonDocument doc(heartbeat);
		QString body = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
		// 使用现有的 send_data 方法发送心跳（函数内部会检查 socket 状态）
		this->send_data(HEARTBEAT_PACKET, body);
	});
	this->heartbeat_packet.start();
}
