#include "chatpage.h"
#include <QDebug>

#include "chatitembase.h"
#include "global.h"
#include "PictureBubble.h"
#include "StyleManager.h"
#include "TextBubble.h"

ChatPage::ChatPage(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::ChatPageClass())
{
	ui->setupUi(this);

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
}


void ChatPage::connect_sig()
{
	connect(this->ui->QP_send, &QPushButton::clicked, this, &ChatPage::on_send_btn_clicked);
}

void ChatPage::on_send_btn_clicked()
{
	// 发送按钮点击事件
	auto text_edit = this->ui->textEdit;
	ChatRole role = ChatRole::SELF;
	QString user_name = QStringLiteral("陈守阳");
	QString user_icon = ":/res/head_1.png";
	const QVector<MsgInfo>& msg_list = text_edit->getMsgList();
	for (int i=0;i<msg_list.size();++i)
	{
		QString type = msg_list[i].msgFlag;
		ChatItemBase* item = new ChatItemBase(role);
		item->set_user_name(user_name);
		item->set_user_icon(QPixmap(user_icon));
		QWidget* Bubble = nullptr;
		if (type=="text")
		{
			Bubble = new TextBubble(role, msg_list[i].content);
			qDebug() << msg_list[i].content;
		}
		else if (type == "image")
		{
			Bubble = new PictureBubble(QPixmap(msg_list[i].content), role);
			qDebug() << msg_list[i].content;
		}
		else if (type == "file")
		{
		}
		if (Bubble != nullptr)
		{
			item->set_widget(Bubble);
			this->ui->chat_view->append_item(item);
		}
	}
}


ChatPage::~ChatPage()
{
	delete ui;
}

