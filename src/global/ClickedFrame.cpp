#include "ClickedFrame.h"
#include <QMouseEvent>
#include <QDebug>
ClickedFrame::ClickedFrame(QWidget* parent)
	: QFrame(parent)
{
	setFrameStyle(QFrame::Box | QFrame::Plain);//设置边框样式为实线
	setLineWidth(2);
}

void ClickedFrame::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		setStyleSheet("background-color: lightgray;"); // 按下时改变背景色
	}
	QFrame::mousePressEvent(event); // 调用基类的事件处理器
}

void ClickedFrame::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		setStyleSheet(""); // 释放时恢复原始背景色
		QString object_name = this->objectName();
		if (object_name == "Button_Pass")//如果是通过frame
			emit sig_clicked(true); // 发出点击信号
		else if (object_name == "Button_Refuse")//如果是拒绝frame
			emit sig_clicked(false); // 发出点击信号
	}
	QFrame::mouseReleaseEvent(event); // 调用基类的事件处理器
}