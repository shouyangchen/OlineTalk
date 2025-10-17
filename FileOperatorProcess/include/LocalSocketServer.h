#ifndef LOCALSOCKETSERVER_H
#define LOCALSOCKETSERVER_H
#include <QAbstractSocket>
#include <QLocalServer>
#include <QByteArray>

class LocalSocketServer : public QObject {
	Q_OBJECT
private:
	QLocalServer* server;
public:
	LocalSocketServer(QObject* parent=nullptr);
	void listen();//监听
	void processData(const QByteArray& data); //处理数据
	~LocalSocketServer()override=default;
};
#endif
