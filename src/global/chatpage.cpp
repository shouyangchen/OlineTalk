#include "chatpage.h"
#include <QDebug>
#include "StyleManager.h"

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
}

ChatPage::~ChatPage()
{
	delete ui;
}

