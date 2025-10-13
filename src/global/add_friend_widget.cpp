#include "add_friend_widget.h"

#include "the_user_icon_mgr.h"


add_friend_widget::add_friend_widget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::add_friend_widgetClass())
{
	ui->setupUi(this);
}


void add_friend_widget::set_search_info(std::shared_ptr<SearchInfo> si)
{
	this->search_info_m = std::move(si);
	this->ui->user_name->setText(this->search_info_m->_name);
	this->ui->user_id->setText(QString::number(this->search_info_m->_uid));
	auto icon_data = QByteArray::fromBase64(this->search_info_m->_icon.toLocal8Bit());//将base64编码的字符串转换为字节数组并且解码
	QPixmap icon;
	icon.loadFromData(icon_data);
	//设置头像
	if (!icon.isNull())
	{
		auto width = this->ui->user_icon->width();
		auto height = this->ui->user_icon->height();
		this->ui->user_icon->setPixmap(icon.scaled(width, height, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));//按比例缩放并且平滑转换
	}
	else
	{
		auto width = this->ui->user_icon->width();
		auto height = this->ui->user_icon->height();
		QPixmap default_icon = the_user_icon_mgr::getInstance()->get_user_icon(QString{ "default_user_icon" }, QString{ "default_user_email" });//获取默认头像
		this->ui->user_icon->setPixmap(default_icon.scaled(width, height, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));//按比例缩放并且平滑转换
	}
}


add_friend_widget::~add_friend_widget()
{
	delete ui;
}

