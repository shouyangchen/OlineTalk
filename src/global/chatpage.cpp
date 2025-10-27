#include "chatpage.h"
#include <qbuffer.h>
#include <QDebug>
#include <QJsonObject>
#include "chatitembase.h"
#include "global.h"
#include "StyleManager.h"
#include "TcpMgr.h"

ChatPage::ChatPage(QWidget *parent)
	: QWidget(parent),now_uid("12345")
	, ui(new Ui::ChatPageClass())
{
	ui->setupUi(this);
	this->file_socket = std::make_shared<QLocalSocket>();//初始本地socket
	if (!StyleManager::getInstance()->loadStyleSheet(this, ":/style/chatpage.qss")) {
		qDebug() << "Failed to load ChatPage stylesheet";
	}
	
	// 设置表情图标
	ui->emjo_lb->setPixmap(
		QPixmap(":/res/smile.png").scaled(25, 25, Qt::KeepAspectRatio, Qt::SmoothTransformation)
	);
	
	// 设置文件图标
	ui->file_lb->setPixmap(
		QPixmap(":/res/filedir.png").scaled(25, 25, Qt::KeepAspectRatio, Qt::SmoothTransformation)
	);
	ui->emjo_lb->setToolTip(u8"发送表情");
	ui->file_lb->setToolTip(u8"发送文件");
	
	// 验证图片是否加载成功
	QPixmap emjoPixmap(":/res/smile.png");
	QPixmap filePixmap(":/res/filedir.png");
	
	if (emjoPixmap.isNull()) {
		qDebug() << "Failed to load emoji icon: :/res/smile.png";
	} else {
		qDebug() << "Emoji icon loaded successfully";
	}
	
	if (filePixmap.isNull()) {
		qDebug() << "Failed to load file icon: :/res/filedir.png";
	} else {
		qDebug() << "File icon loaded successfully";
	}
	
	// 设置标签的最小尺寸以确保图片可见
	ui->emjo_lb->setMinimumSize(25, 25);
	ui->file_lb->setMinimumSize(25, 25);
	
	// 设置标签的缩放模式
	ui->emjo_lb->setScaledContents(true);
	ui->file_lb->setScaledContents(true);
	
	// 设置鼠标悬停效果
	ui->emjo_lb->setCursor(Qt::PointingHandCursor);
	ui->file_lb->setCursor(Qt::PointingHandCursor);
	this->connect_sig();
	this->ui->MultiSelectionHistoryWidget_widget->setVisible(false);
	this->set_file_socket();//连接本地socket
}


void ChatPage::connect_sig()
{
	connect(this->ui->QP_send, &QPushButton::clicked, this, &ChatPage::on_send_btn_clicked);
	connect(this->ui->QP_recived, &QPushButton::clicked, this, &ChatPage::on_received_clicked);
	connect(this->ui->label_cancel, &ClickedLabel::Clicked, [this]
		{
			this->ui->textEdit->setVisible(true);
			this->ui->tool_widget->setVisible(true);
			this->ui->QP_recived->setVisible(true);
			this->ui->QP_send->setVisible(true);
			this->ui->MultiSelectionHistoryWidget_widget->setVisible(false);
			this->ui->chat_view->chatHistoryView->clearSelection();
			this->ui->chat_view->chatHistoryView->setSelectionMode(QAbstractItemView::NoSelection);
		});
}

void ChatPage::on_send_btn_clicked()
{
	//// 发送按钮点击事件
	auto text_edit = this->ui->textEdit;
	ChatMessage message;
	message.sender = "SELF";
	const QVector<ChatMessage>& msg_list = text_edit->getMsgList();
	auto model = dynamic_cast<ChatHistoryDataModel*>(this->ui->chat_view->get_model());
	auto user_id = model->getUserID();
	for (auto& item : msg_list)
	{
		const_cast<ChatMessage&>(item).sender = "SELF";
		emit model->sig_send_the_message(item, user_id);
		QString data;
		this->serialization_message(item, data);//序列化消息
		qDebug() << "Serialized message data:" << data;
		TcpMgr::getInstance()->send_data(Message_id::SEND_MESSAGE_HANDLER, data);//发送消息给聊天服务器
	}
	
}

void ChatPage::on_received_clicked()
{
	//// 发送按钮点击事件
	auto text_edit = this->ui->textEdit;
	ChatMessage message;
	message.sender = "OTHER";
	const QVector<ChatMessage>& msg_list = text_edit->getMsgList();
	auto model = dynamic_cast<ChatHistoryDataModel*>(this->ui->chat_view->get_model());
	auto user_id = model->getUserID();
	for (auto& item : msg_list)
	{
		emit model->sig_send_the_message(item, user_id);
	
	}
	

}

void ChatPage::serialization_message(const ChatMessage& msg, QString& out_data)
{
 //解析msg里面的内容然写入out_data
	QByteArray data;
	QDataStream out(&data, QDataStream::WriteOnly);
	QBuffer buffer;
	QJsonObject json_object;
	json_object["to_uid"] =this->now_uid.toDouble();//设置发送对象
	json_object["timestamp"] = QDateTime::currentSecsSinceEpoch();//设置时间戳
	if (msg.message_type=="text")//如果消息类型为文本
	{
		json_object["message"] =QString::fromUtf8( msg.message_context.value<QString>().toUtf8());
		json_object["message_type"] = QString("text");
	}
	if (msg.message_type=="image")
	{
		//将图片转换为base64字符串
		buffer.open(QIODevice::WriteOnly);
		msg.pixmap.save(&buffer, "PNG");//将缩略图保存到buffer中
		QByteArray byte_array = buffer.data();
		QString base64_str = QString::fromLatin1(byte_array.toBase64());
		json_object["message"] = base64_str;//缩略图的base64字符串
		json_object["message_type"] = QString("image");
		QString image_url = msg.message_context.toString();//图片的本地路径
		//通过本地socket给文件操作进程发送图片传输请求
		QJsonObject image_object;
		image_object["file_name"] = image_url;
		image_object["to_uid"] = this->now_uid;
		QByteArray image_data = QJsonDocument(image_object).toJson();
		this->file_socket->write(image_data);//发送图片传输请求
	}
	if (msg.message_type=="file")//如果消息为文件
	{
		//通过本地socket给文件操作进程发送文件传输请求
		QJsonObject file_object;
		file_object["to_uid"] = this->now_uid;
		file_object["file_name"] = msg.message_context.toString();
		QByteArray file_data = QJsonDocument(file_object).toJson();
		this->file_socket->write(file_data);
	}
	QJsonDocument json_doc(json_object);
	out_data = QString::fromUtf8(json_doc.toJson());//将json对象转换为字符串
}




void ChatPage::on_history_id_changed(const QString&uid)
{
	this->now_uid = uid;
	qDebug() << "ChatPage now_uid changed to:" << this->now_uid;
}


void ChatPage::set_file_socket()
{
	if (this->file_socket->state()==QLocalSocket::UnconnectedState)
	{
#ifdef Q_OS_UNIX
		QString socket_path = QDir::toNativeSeparators("/tmp/FileServer");//本地socket路径
#endif

#ifdef Q_OS_WIN
		QString socket_path = R"(\\.\pipe\FileServer)";//本地socket路径
#endif

#ifdef Q_OS_MAC
		QString socket_path = QDir::toNativeSeparators("/tmp/FileServer");//本地socket路径
#endif
		this->file_socket->connectToServer(socket_path);//连接到本地socket服务器
		if (!this->file_socket->waitForConnected(3000))//等待连接成功
		{
			qDebug() << "Failed to connect to local file socket server:" << this->file_socket->errorString();
		}
		else
		{
			qDebug() << "Connected to local file socket server successfully.";
		}
	}
}

ChatPage::~ChatPage()
{
	delete ui;
}

