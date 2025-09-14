#include <qevent.h>
#include "StatusChangeButton.h"
StatusChangeButton::StatusChangeButton(QWidget* parent):QPushButton(parent)
{
	
}

StatusChangeButton::StatusChangeButton(const QIcon& icon, const QString& text, QWidget* parent):QPushButton(icon,text,parent)
{
	
}

StatusChangeButton::StatusChangeButton(const QString& text, QWidget* parent):QPushButton(text,parent)
{
	
}

StatusChangeButton::~StatusChangeButton()
{
	
}

void StatusChangeButton::enterEvent(QEnterEvent* event)
{
	if (this->rect().contains(event->position().toPoint()))
	{
		this->setIcon(QIcon(":/res/add_friend_hover.png"));
		setCursor(Qt::CursorShape::PointingHandCursor);
		QPushButton::enterEvent(event);
	}
	QPushButton::enterEvent(event);
}

void StatusChangeButton::leaveEvent(QEvent* event)
{
	this->setIcon(QIcon(":/res/add_friend_normal.png"));
	this->setCursor(Qt::CursorShape::ArrowCursor);
	QPushButton::leaveEvent(event);
}

void StatusChangeButton::mousePressEvent(QMouseEvent* event)
{
	this->setIcon(QIcon(":/res/addtip.png"));
	QPushButton::mousePressEvent(event);
}


void StatusChangeButton::mouseReleaseEvent(QMouseEvent* event)
{
	this->setIcon(QIcon(":/res/add_friend_normal.png"));
	QPushButton::mouseReleaseEvent(event);
}


