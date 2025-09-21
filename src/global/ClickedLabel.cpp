#include "ClickedLabel.h"

#include <QMouseEvent>

ClickedLabel::ClickedLabel(QWidget* parent) : QLabel(parent) 
{
    // 设置一些默认属性
    this->setAlignment(Qt::AlignCenter);
    this->setScaledContents(false);
}

ClickedLabel::~ClickedLabel()
{
}

void ClickedLabel::mousePressEvent(QMouseEvent* event)
{
	if(event->button() == Qt::LeftButton)
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

