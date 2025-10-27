#include "add_friend_widget.h"
#include "the_user_icon_mgr.h"
#include "user_info_mgr.h"
#include "TcpMgr.h"

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
	this->search_info_m = std::move(si);
	this->ui->user_name->setText(this->search_info_m->name);
	this->ui->user_id->setText(this->search_info_m->uid);
	qDebug() << "设置添加好友界面用户信息，用户名:" << this->search_info_m->name << "用户ID:" << this->search_info_m->uid;
	auto icon_data = QByteArray::fromBase64(this->search_info_m->icon.toLocal8Bit());//将base64编码的字符串转换为字节数组并且解码
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
		the_user_icon_mgr::getInstance()->set_user_icon(this->search_info_m->uid, QString{}, icon);//缓存用户头像
		auto width = this->ui->user_icon->width();
		auto height = this->ui->user_icon->height();
		QPixmap default_icon = the_user_icon_mgr::getInstance()->get_user_icon(QString{ "default_user_icon" }, QString{ "default_user_email" });//获取默认头像
		this->ui->user_icon->setPixmap(default_icon.scaled(width, height, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));//按比例缩放并且平滑转换
	}
}


void add_friend_widget::connect_sig()
{
	using value_clicked_sig_ptr = void (ClickedFrame::*)(const bool&);
	value_clicked_sig_ptr sig_ptr = &ClickedFrame::sig_clicked;

	connect(this->ui->Button_Cancel, sig_ptr, [this](const bool clicked)
		{
			Q_UNUSED(clicked)
				this->hide();
			this->close();
			this->deleteLater();// 关闭并且删除自己
		});
	connect(this->ui->Button_Pass, sig_ptr, [this](const bool clicked)
		{
			Q_UNUSED(clicked)
				qDebug() << "发送添加好友请求，用户ID:" << this->search_info_m->uid;
			//发送添加好友请求
			if (this->search_info_m->uid.isEmpty())
			{
				return;
			}
			qDebug() << "user id is " << user_info_mgr::getInstance(
				QPixmap{}, QString{}, QString{}, std::uint64_t{})->get_user_id();
			QJsonObject json_obj;
			json_obj["application_user_id"] = static_cast<double>(user_info_mgr::getInstance(
				QPixmap{}, QString{}, QString{}, std::uint64_t{})->get_user_id());
			json_obj["target_user_id"] = this->search_info_m->uid.toDouble();
			json_obj["message"] = QString::fromUtf8(this->ui->applcation_message->toPlainText().toUtf8());//进行utf8编码
			auto json_doc = QJsonDocument(json_obj);
			auto json_str = json_doc.toJson(QJsonDocument::Compact);
			TcpMgr::getInstance()->send_data(ADD_FRIEND_REQUEST, QString::fromUtf8(json_str));
			this->hide();
			this->close();
			this->deleteLater();// 关闭并且删除自己
		});
}
		
add_friend_widget::~add_friend_widget()
{
	delete ui;
}