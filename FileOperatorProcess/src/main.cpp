//用于处理文件操作和发送文件接受文件的子进程
#include <QCoreApplication>
#include <QLocalSocket>
#include <QDebug>

#include "FileOperatorProcess/include/LocalSocketServer.h"

int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);
	qDebug() <<u8"子进程启动!";
	LocalSocketServer server;
	server.listen();//开始监听本地socket连接
	return a.exec();
}
