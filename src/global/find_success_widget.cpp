#include "find_success_widget.h"
#include "the_user_icon_mgr.h"
#include <QPointer>

find_success_widget::find_success_widget(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::find_success_widgetClass())
{
	ui->setupUi(this);
	this->setWindowTitle(u8"添加好友");
	this->setWindowFlags(this->windowFlags()|Qt::FramelessWindowHint);
	//先设置默认的头像当头像传输过来之后在去修改头像
	QPixmap user_icon = the_user_icon_mgr::getInstance()->get_user_icon(QString{ "default_user_icon" }, QString{ "default_user_email" });
	this->ui->friend_icon->setPixmap(user_icon.scaled(this->ui->friend_icon->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
	connect(this->ui->Exit, &ClickedLabel::Clicked, [this]
		{
			emit this->will_close();
		});
	connect(this->ui->add_friend_button, &QPushButton::clicked, this, &find_success_widget::on_the_add_friend_clicked);
}


void find_success_widget::set_serach_info(std::shared_ptr<SearchInfo> si)
{
	this->search_info_m = std::move(si);
	this->ui->friend_name->setText(this->search_info_m->_name);
}


void find_success_widget::on_the_add_friend_clicked()
{
	qDebug() <<u8"添加好友界面被触发！";
	emit this->will_close();
	add_friend_widget* afw = new add_friend_widget(nullptr);
	afw->set_search_info(this->search_info_m);
	afw->show();
}


find_success_widget::~find_success_widget()
{
	delete ui;
}

