#include "chatview.h"
#include "StyleManager.h"

#include <QScrollBar>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QEvent>
#include <QDebug>

/*该界面显示聊天记录的思想为利用QVBoxLayout和QScroll_area
 *我们先利用一个大的QVBoxLayout，这样首先保证我们的布局整体为一个垂直布局
 *然后呢我门添加一个QScroll_area同时将他添加进入我们的垂直布局当中让其成为主布局
 *然然创建一个QWidget并且设置为自动填充让其子自动将大小设置为父窗口的大小
 *我们创建它的目的是为了实现聊天背景功能同时也是为了提高后续的功能开发便利
 *然后我们在添加一个widget到chat_bg当中去让他作为聊天的主要显示框架，同时让他以垂直布局的形式添加进入chat_bg
 *
 */

ChatView::ChatView(QWidget*parent)
	: QWidget(parent),is_Appended(false)
{
	// 加载ChatView专用样式表
	if (!StyleManager::getInstance()->loadStyleSheet(this, ":/style/chatview.qss")) {
		qDebug() << "Failed to load ChatView stylesheet";
	}

	QVBoxLayout* p_main_layout = new QVBoxLayout();
	this->setLayout(p_main_layout);
	p_main_layout->setContentsMargins(0, 0, 0, 0);
	
	this->p_scroll_area = new QScrollArea();
	this->p_scroll_area->setObjectName("chat_area");
	p_main_layout->addWidget(p_scroll_area);
	
<<<<<<< HEAD
	QWidget* w = new QWidget(this);//聊天背景
	w->setObjectName("chat_bg");
	w->setAutoFillBackground(true);//自动填充
	
	QVBoxLayout* box_layout = new QVBoxLayout();
	box_layout->addWidget(new QWidget(), 10000);//创建一个新的垂直布局并且添加一个widget进入让其填充满整个布局
	w->setLayout(box_layout);//将聊天背景的布局设置为垂直布局
=======
	QWidget* w = new QWidget(this);
	w->setObjectName("chat_bg");
	w->setAutoFillBackground(true);//自动填充
	
	QVBoxLayout* p_Hlayout = new QVBoxLayout();
	p_Hlayout->addWidget(new QWidget(), 10000);//创建一个新的垂直布局并且添加一个widget进入让其填充满整个布局
	w->setLayout(p_Hlayout);//将聊天背景的布局设置为垂直布局
>>>>>>> 784920c682c11f872a4f6f362076919e7000b57a
	this->p_scroll_area->setWidget(w);//将聊天背景添加进入scroll_area里因为聊天背景我们设置为了自动填充那么scroll_area的就会被char_bg自动填充满自动位置其大小为父窗口的大小

	p_scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	auto pVscrollBar = p_scroll_area->verticalScrollBar();

	connect(pVscrollBar, &QScrollBar::rangeChanged, this, &ChatView::on_VScroll_bar_moved);

	QHBoxLayout* h_layout2 = new QHBoxLayout();
	h_layout2->addWidget(pVscrollBar, 0, Qt::AlignRight);
	h_layout2->setContentsMargins(0,0,0,0);
<<<<<<< HEAD
	this->p_scroll_area->setLayout(h_layout2);//将竖直滚动条设置到侧边的水平布局
	pVscrollBar->setHidden(true);//QScorllArea的垂直滚动条隐蔽掉因为侧边的水平布局有了一个

	p_scroll_area->setWidgetResizable(true);//将自适应设置为true为了隐藏滚动条
=======
	this->p_scroll_area->setLayout(h_layout2);
	pVscrollBar->setHidden(true);

	p_scroll_area->setWidgetResizable(true);
>>>>>>> 784920c682c11f872a4f6f362076919e7000b57a
	p_scroll_area->installEventFilter(this);
	
	qDebug() << "ChatView initialized with stylesheet support";
}

void ChatView::append_item(QWidget* item)
{
	QVBoxLayout* vl = qobject_cast<QVBoxLayout*>(this->p_scroll_area->widget()->layout());
	vl->insertWidget(vl->count() - 1, item);
	is_Appended = true;
}

QWidget* ChatView::create_message_bubble(const QString& message, const QString& sender, const QString& timestamp)
{
<<<<<<< HEAD
	QWidget* messageWidget = new QWidget();//创建一个widget用于将返回给外部调用注意该widget是没有父对象的，即内存没有被管理
	messageWidget->setProperty("messageType", "bubble");//设置对象的元信息
	messageWidget->setProperty("sender", sender);
	
	QVBoxLayout* layout = new QVBoxLayout(messageWidget);//创建一个垂直布局在widget里
=======
	QWidget* messageWidget = new QWidget();
	messageWidget->setProperty("messageType", "bubble");
	messageWidget->setProperty("sender", sender);
	
	QVBoxLayout* layout = new QVBoxLayout(messageWidget);
>>>>>>> 784920c682c11f872a4f6f362076919e7000b57a
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(2);
	
	// 时间戳标签
	if (!timestamp.isEmpty()) {
		QLabel* timeLabel = new QLabel(timestamp);
		timeLabel->setProperty("labelType", "timestamp");
		timeLabel->setAlignment(Qt::AlignCenter);
		layout->addWidget(timeLabel);
	}
	
	// 消息内容布局
<<<<<<< HEAD
	QHBoxLayout* messageLayout = new QHBoxLayout();//创建消息的水平布局
=======
	QHBoxLayout* messageLayout = new QHBoxLayout();
>>>>>>> 784920c682c11f872a4f6f362076919e7000b57a
	
	if (sender == "self") {
		messageLayout->addStretch(); // 右对齐
	}
	
	// 消息气泡
<<<<<<< HEAD
	QLabel* bubbleLabel = new QLabel(message);//创建一个label将信息写入label
	bubbleLabel->setWordWrap(true);//启动自动换行，原理根据内容的像素点判断是否超过label的固定宽度
	bubbleLabel->setMaximumWidth(300);
	bubbleLabel->setMinimumWidth(50);
	
	messageLayout->addWidget(bubbleLabel);//将信息显示label加入消息显示垂直布局
=======
	QLabel* bubbleLabel = new QLabel(message);
	bubbleLabel->setWordWrap(true);
	bubbleLabel->setMaximumWidth(300);
	bubbleLabel->setMinimumWidth(50);
	
	messageLayout->addWidget(bubbleLabel);
>>>>>>> 784920c682c11f872a4f6f362076919e7000b57a
	
	if (sender == "other") {
		messageLayout->addStretch(); // 左对齐
	}
	
<<<<<<< HEAD
	layout->addLayout(messageLayout);//将消息的水平布局填入整体垂直布局
=======
	layout->addLayout(messageLayout);
>>>>>>> 784920c682c11f872a4f6f362076919e7000b57a
	
	return messageWidget;
}

QWidget* ChatView::create_system_message(const QString& message)
{
	QWidget* systemWidget = new QWidget();
	systemWidget->setProperty("messageType", "system");
	
	QHBoxLayout* layout = new QHBoxLayout(systemWidget);
	layout->setContentsMargins(0, 0, 0, 0);
	
	QLabel* messageLabel = new QLabel(message);
	messageLabel->setAlignment(Qt::AlignCenter);
	messageLabel->setWordWrap(true);
	
	layout->addWidget(messageLabel);
	
	return systemWidget;
}

void ChatView::add_message(const QString& message, const QString& sender, const QString& timestamp)
{
	QWidget* messageItem = create_message_bubble(message, sender, timestamp);
	append_item(messageItem);
}

void ChatView::add_system_message(const QString& message)
{
	QWidget* systemItem = create_system_message(message);
	append_item(systemItem);
}

void ChatView::prepend_item(QWidget* item)
{
	
}

void ChatView::insert_ietm(QWidget* before,QWidget*item)
{
	
}

void ChatView::on_VScroll_bar_moved(int min, int max)
{
	if (this->is_Appended)
	{
		auto VScrollBar = this->p_scroll_area->verticalScrollBar();
		VScrollBar->setSliderPosition(VScrollBar->maximum());
		QTimer::singleShot(500, [this]
			{
				is_Appended = false;
			});
	}
}

bool ChatView::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type()==QEvent::Enter&&watched==this->p_scroll_area)
	{
		auto VScrollBar = this->p_scroll_area->verticalScrollBar();
		VScrollBar->setHidden(p_scroll_area->verticalScrollBar()->maximum() == 0);//如果最大值为0说明没有滚动条
	}
	else if (event->type()==QEvent::Leave&&watched==this->p_scroll_area)
	{
		auto VScrollBar = this->p_scroll_area->verticalScrollBar();
		VScrollBar->setHidden(true);
	}
	return QWidget::eventFilter(watched, event);
}

ChatView::~ChatView()
{}

