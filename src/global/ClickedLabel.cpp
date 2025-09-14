#include "ClickedLabel.h"
#include <QMouseEvent>


ClickedLabel::ClickedLabel(QObject*parent) {
	
}

ClickedLabel::~ClickedLabel()
{

}

void ClickedLabel::mousePressEvent(QMouseEvent* event)
{
	if(event->button()==Qt::LeftButton)
	{
		if (this->rect().contains(event->pos()))
		{
			emit this->Clicked();
			event->accept();
		}
	}
	else
		QLabel::mousePressEvent(event);
}