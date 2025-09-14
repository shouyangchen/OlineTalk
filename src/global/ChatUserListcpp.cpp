#include "ChatUserList.h"
ChatUserList::ChatUserList(QWidget* parent):QListWidget(parent)
{
	Q_UNUSED(parent);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//安装事件过略器
	this->viewport()->installEventFilter(this);
}

bool ChatUserList::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == this->viewport())
	{
		if (event->type() == QEvent::Enter)
		{
			this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		}
		else if (event->type() == QEvent::Leave) {
			//鼠标离开，隐藏滚动条
			this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		}
	}
	//检查事件是否为鼠标滚轮事件
	if (watched == this->viewport() && event->type() == QEvent::Wheel)
	{
		QWheelEvent* wheel_event = static_cast<QWheelEvent*>(event);
		int numDegrees = wheel_event->angleDelta().y() / 8;
		int numSteps = numDegrees / 15;//计算滚动步数

		//设置滚动幅度
		this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);

		//检查是否滚动到底
		QScrollBar* scroll_bar = this->verticalScrollBar();
		int maxScrollValue = scroll_bar->maximum();
		int currentValue = scroll_bar->value();
		//int pageSize=10;//每页加载的人数
		if (maxScrollValue - currentValue <= 0)
		{
			//滚动到底，加载新的联系人
			qDebug() << "load more chat user!";
			emit sig_loading_chat_user();
		}
		return true;
	}
	return QListWidget::eventFilter(watched, event);
}

