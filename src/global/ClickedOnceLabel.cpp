#include "ClickedOnceLabel.h"

#include <QMouseEvent>

ClickedOnceLabel::ClickedOnceLabel(QWidget* parent): QLabel(parent)
{
	
}

void ClickedOnceLabel::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->buttons()==Qt::LeftButton)
	{
		emit this->clicked(this->text());
		return;
	}
	return QLabel::mouseReleaseEvent(event);
}
