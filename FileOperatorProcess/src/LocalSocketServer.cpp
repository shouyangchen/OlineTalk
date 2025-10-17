#include "FileOperatorProcess/include/LocalSocketServer.h"
#include "QDebug"
#include <QLocalSocket>
#include <QVariant>
#include <QDataStream>
#include <QDir>

#include "FileOperatorProcess/include/TaskQueue.h"
LocalSocketServer::LocalSocketServer(QObject* parent)
{
	this->server = new QLocalServer(this);
	if (QLocalServer::removeServer("FileServer"))
	{
		qDebug() << "Removed existing server socket.";
	}
	this->server->setSocketOptions(QLocalServer::UserAccessOption);//设置为只有用户自己可以访问
	this->server->setMaxPendingConnections(1);//设置最大连接数为1
	connect(this->server, &QLocalServer::newConnection, [this]
		{
			auto* socket = this->server->nextPendingConnection();
			if (!socket) {
				qWarning() << "No pending connection available.";
				return;
			}
			//当连接断开时，删除socket对象
			connect(socket, &QLocalSocket::disconnected, socket, &QObject::deleteLater);
			//当有数据可读时，读取数据并处理
			connect(socket, &QLocalSocket::readyRead, this, [this, socket]() {
				QByteArray data = socket->readAll();
				this->processData(data);
			});
			});
}



void LocalSocketServer::processData(const QByteArray& data)
{
	QByteArray buffer = data; 
	QDataStream in(&buffer, QIODevice::ReadOnly);
	Request req;
	in >> req;//反序列化请求对象
	if (in.status() != QDataStream::Ok) {
		qWarning() << "Failed to deserialize Request from data";
		return;
	}
	TaskQueue::getInstance()->push_back(req);//将请求加入任务队列
}




void LocalSocketServer::listen()
{
#ifdef Q_OS_UNIX
	QString socket_path = QDir::toNativeSeparators("/tmp/FileServer");//本地socket路径
#endif

#ifdef Q_OS_WIN
	QString socket_path = R"(\\.\pipe\FileServer)";//本地socket路径
#endif

#ifdef Q_OS_MAC
	QString socket_path = QDir::toNativeSeparators("/tmp/FileServer");//本地socket路径
#endif
	if (!this->server->listen(socket_path))
	{
		qDebug() << "Server failed to start:" << this->server->errorString();
	}
	else
	{
		qDebug() << "Server listening on" << this->server->fullServerName();
	}
}