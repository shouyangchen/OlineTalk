#include "FileOperatorProcess/include/TaskHandler.h"
#include "QDebug"
TaskHandler::TaskHandler(QObject* parent, quint16 receive_port):QObject(parent), receive_port_m(receive_port)
{
	this->sender_socket = new QTcpSocket(this);
	this->receiver_socket = new QTcpServer(this);
	this->receiver_socket->setMaxPendingConnections(100);//设置最大连接数
	this->receiver_socket->setListenBacklogSize(100);//设置监听队列大小
	this->receiver_socket->listen(QHostAddress::Any, receive_port);//监听所有地址的指定端口
	connect(this->receiver_socket, &QTcpServer::newConnection, [this]() {
		QTcpSocket* client_socket = this->receiver_socket->nextPendingConnection();
		QString client_address = client_socket->peerAddress().toString();
		//向聊天服务器验证该客户端是否合法，且获取他的uid后再缓存socket
		//TODO:验证客户端合法性代码
		//TODO:获取客户端uid代码
		//TODO:如果验证不合法则断开连接否者继续
		//TODO:将uid与client_socket缓存起来方便后续使用
		});
}



void TaskHandler::handleTask(const Request& req)
{
	switch (req.request_type)
	{
		case Request::REQUEST_SEND_FILE:
		{
			this->setSendHost(req);
			this->setSendPort(req.target_port);
			//读取文件数据处理
			emit processDone(req);
			break;
		}

		case Request::REQUEST_DOWN_LOAD_FILE:
		{
			this->setReceiveHost(req.target_host);
			this->setReceivePort(req.target_port);
			//准备接收文件数据处理
			emit processDone(req);
			break;
		}

		case Request::REQUEST_NEED_UPDATE_HOST:
			{
			this->setSendHost(req);
			}
	}
}

void TaskHandler::setSendHost(const Request&req)
{
	auto uid = req.uid;
	this->send_host_m = req.target_host;
	this->sender_socket->connectToHost(send_host_m, 6868, QIODeviceBase::ReadWrite);//默认端口6868
}

void TaskHandler::setSendPort(quint16 port)
{
	this->send_port_m = port;
}

void TaskHandler::setReceiveHost(const QString& host)
{
	this->receive_host_m = host;
}

void TaskHandler::setReceivePort(quint16 port)
{
	this->receive_port_m = port;
}

void TaskHandler::sendFile(const QByteArray& file_data)
{

}

void TaskHandler::receiveFile(const QString& save_path, qint64 file_size)
{

}

void TaskHandler::cutFile(const QString file_path, quint16& offset)
{
}

void TaskHandler::spliceFile(QString file_path, quint16 offset, const QByteArray& file_data)
{
#ifdef Q_OS_UNIX
#endif
#ifdef Q_OS_WIN
#endif
}


