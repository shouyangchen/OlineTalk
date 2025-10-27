#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <QWidget>
#include <QLocalSocket>
#include "ui_chatpage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ChatPageClass; };
QT_END_NAMESPACE

class ChatPage : public QWidget
{
	Q_OBJECT
private:
	friend class ChatHistoryView;
	friend class New_ChatView;
	friend class chat_ui;
	std::shared_ptr<QLocalSocket> file_socket;//与文件操作进程进行本地沟通的socket
public:
	ChatPage(QWidget *parent = nullptr);
	~ChatPage()override;
public slots:
	void on_send_btn_clicked();
	void on_received_clicked();
	void serialization_message(const ChatMessage&, QString&);
	void on_history_id_changed(const QString& uid);
	//void on_emjo_lb_clicked();
	//void on_file_lb_clicked();
private:
	Ui::ChatPageClass *ui;
	void connect_sig();
	QString now_uid;
	void set_file_socket();
};

#endif