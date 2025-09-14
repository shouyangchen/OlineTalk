#pragma once

#include <QWidget>
#include "ui_chat_user_display.h"
#include "ListItemBase.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Chat_User_DisplayClass; };
QT_END_NAMESPACE

class Chat_User_Display : public ListItemBase
{
	Q_OBJECT

public:
	explicit Chat_User_Display(QWidget *parent = nullptr);
	~Chat_User_Display();

	QSize sizeHint()const override;
	void set_info(QString name, QString head, QString msg);

private:
	Ui::Chat_User_DisplayClass *ui;
	QString name;
	QString head;
	QString msg;
};

