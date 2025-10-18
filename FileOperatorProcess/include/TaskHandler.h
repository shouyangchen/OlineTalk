#ifndef TASKHANDLER_H
#define TASKHANDLER_H
#include <QDir>
#include <QObject>
#include <QFile>
#include <QCache>
#include <QTcpSocket>
#include <QTcpServer>
#include "TaskQueue.h"
#ifdef Q_OS_UNIX // Linux系统macOS等类Unix系统
#include <x86_64-linux-gnu/sys/fcntl.h>
#include <x86_64-linux-gnu/sys/mman.h>
#include <x86_64-linux-gnu/sys/stat.h>
#include <x86_64-linux-gnu/sys/unistd.h>
#endif

#ifdef Q_OS_MAC // macOS系统

#endif



#ifdef Q_OS_WIN // Windows系统
#include <windows.h>
#endif

class TaskHandler : public QObject ,public singleton<TaskHandler>{
	Q_OBJECT
private:
	friend class singleton<TaskHandler>;
	QTcpSocket* sender_socket;//用于发送数据的socket
	QTcpServer* receiver_socket;//用于接收数据的socket
	QString send_host_m;//发送主机地址
	quint16 send_port_m;//发送端口
	QString receive_host_m;//接收主机地址
	quint16 receive_port_m;//接收端口
	QCache<QString, QTcpSocket> socket_cache_m;//socket缓存用户id与socket的映射
	TaskHandler(QObject* parent, quint16 receive_port = 6868);
public:
	TaskHandler(const TaskHandler& other) = delete;
	TaskHandler& operator=(const TaskHandler& other) = delete;
	TaskHandler(TaskHandler&& other) = delete;
	void handleTask(const Request& req);//处理任务
	void setSendHost(const Request&);//设置发送主机地址
	void setSendPort(quint16);//设置发送端口
	void setReceiveHost(const QString&);//设置接收主机地址
	void setReceivePort(quint16);//设置接收端口
	void sendFile(const QByteArray& file_data);//发送文件
	void receiveFile(const QString& save_path, qint64 file_size);//接收文件
	void cutFile(const QString file_path,quint16& offset);//剪切文件
	void spliceFile(QString file_path, quint16 offset, const QByteArray& file_data);//拼接文件
	~TaskHandler() override = default;
	signals:
		void processDone(const Request& req);//任务处理完成信号
		void needUpdateHost(const Request&);//当出现连接错误或者对端ip变更时需要更新对端地址
		void errorOccurred(const QString& errorMsg);//发生错误信号
		void fileTransferProgress(qint64 bytesSent, qint64 bytesTotal);//文件传输进度信号
};


#endif
