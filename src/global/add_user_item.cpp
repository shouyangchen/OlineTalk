#include "add_user_item.h"
add_user_item::add_user_item(QWidget *parent)
	: ListItemBase(parent)
	, ui(new Ui::add_user_itemClass())
{
	ui->setupUi(this);
	this->ui->icon_label->setPixmap(QPixmap(":/res/addtip.png").scaled(20,20,Qt::KeepAspectRatio,Qt::SmoothTransformation));
	this->ui->find_label->setPixmap(QPixmap(":/res/right_tip.png").scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	this->set_item_type(ListItemType::ADD_USER_ITEM);
}

add_user_item::~add_user_item()
{
	delete ui;
}

