#include "FileOperatorProcess/include/TaskHandler.h"
#include "QDebug"
TaskHandler::TaskHandler(QObject* parent, quint16 receive_port):QObject(parent), receive_port_m(receive_port)
{
	this->sender_socket = new QTcpSocket(this);
	this->receiver_socket = new QTcpSocket(this);
	this->receiver_socket->bind(QHostAddress::Any, receive_port);
}

void TaskHandler::handleTask(const Request& req)
{
	switch (req.request_type)
	{
		case Request::REQUEST_SEND_FILE:
		{
			this->setSendHost(req.target_host);
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
	}
}

void TaskHandler::setSendHost(const QString& host)
{
	this->send_host_m = host;
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
}


