#pragma once

#include <QWidget>
#include "ui_chatpage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ChatPageClass; };
QT_END_NAMESPACE

class ChatPage : public QWidget
{
	Q_OBJECT

public:
	ChatPage(QWidget *parent = nullptr);
	~ChatPage();
public slots:
	void on_send_btn_clicked();
	//void on_emjo_lb_clicked();
	//void on_file_lb_clicked();
private:
	Ui::ChatPageClass *ui;
	void connect_sig();
};

