#include "add_friend_widget.h"
#include "the_user_icon_mgr.h"

add_friend_widget::add_friend_widget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::add_friend_widgetClass())
{
	ui->setupUi(this);
	this->setWindowTitle(u8"添加好友");
	//设置无边框
	this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);
	QColor bgColor(245, 245, 245); // 设置背景颜色为浅灰色
	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, bgColor);
	this->setPalette(palette);
	this->setAutoFillBackground(true);
	//设置提示文本
	this->ui->applcation_message->setPlaceholderText(u8"请输入验证信息");
	this->connect_sig();//连接信号槽
}


void add_friend_widget::set_search_info(std::shared_ptr<SearchInfo> si)
{
	//this->search_info_m = std::move(si);
	//this->ui->user_name->setText(this->search_info_m->_name);
	//this->ui->user_id->setText(QString::number(this->search_info_m->_uid));
	//auto icon_data = QByteArray::fromBase64(this->search_info_m->_icon.toLocal8Bit());//将base64编码的字符串转换为字节数组并且解码
	//QPixmap icon;
	//icon.loadFromData(icon_data);
	////设置头像
	//if (!icon.isNull())
	//{
	//	auto width = this->ui->user_icon->width();
	//	auto height = this->ui->user_icon->height();
	//	this->ui->user_icon->setPixmap(icon.scaled(width, height, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));//按比例缩放并且平滑转换
	//}
	//else
	//{
	//	auto width = this->ui->user_icon->width();
	//	auto height = this->ui->user_icon->height();
	//	QPixmap default_icon = the_user_icon_mgr::getInstance()->get_user_icon(QString{ "default_user_icon" }, QString{ "default_user_email" });//获取默认头像
	//	this->ui->user_icon->setPixmap(default_icon.scaled(width, height, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));//按比例缩放并且平滑转换
	//}
}


void add_friend_widget::connect_sig()
{
	using value_clicked_sig_ptr = void (ClickedFrame::*)(const bool&);
	value_clicked_sig_ptr sig_ptr = &ClickedFrame::sig_clicked;

	connect(this->ui->Button_Cancel, sig_ptr, [this](const bool clicked)
		{
			Q_UNUSED(clicked)
			qDebug() << u8"申请好友界面取消被触发";
			this->hide();
			this->close();
			this->deleteLater();// 关闭并且删除自己
		});
}


add_friend_widget::~add_friend_widget()
{
	delete ui;
}

