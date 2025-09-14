#include "chat_user_display.h"

Chat_User_Display::Chat_User_Display(QWidget *parent)
	: ListItemBase(parent)
	, ui(new Ui::Chat_User_DisplayClass())
{
	ui->setupUi(this);
	ListItemBase::set_item_type(ListItemType::CHAT_USER_ITEM);
}

void Chat_User_Display::set_info(QString name, QString head, QString msg)
{
	this->name = name;
	this->head = head;
	this->msg = msg;

	QPixmap pixmap(head);
	this->ui->user_icon_lb->setPixmap(pixmap.scaled(this->ui->user_icon_lb->size(),
		Qt::KeepAspectRatio, Qt::SmoothTransformation));
	this->ui->user_icon_lb->setScaledContents(true);
	this->ui->user_name_lb->setText(name);
	this->ui->message_lb->setText(msg);
}


QSize Chat_User_Display::sizeHint() const
{
	return { 200, 50 };
}

Chat_User_Display::~Chat_User_Display()
{
	delete ui;
}

