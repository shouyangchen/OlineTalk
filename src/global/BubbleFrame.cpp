#include "BubbleFrame.h"
#include <QPainter>
const int WIDTH_SANJIAO = 8;//三角形的底边像素长度
BubbleFrame::BubbleFrame(ChatRole role, QWidget* parent):QFrame(parent),
role_m(role),margin_m(3)
{
	this->h_layout_m = new QHBoxLayout();
	if (role == ChatRole::SELF)
		this->h_layout_m->setContentsMargins(margin_m, margin_m, WIDTH_SANJIAO + margin_m, margin_m);//设置边距b并且预留出绘制三角的边距
	else
		h_layout_m->setContentsMargins(WIDTH_SANJIAO + margin_m, margin_m, margin_m, margin_m);
	this->setLayout(this->h_layout_m);//设置水平布局
}

void BubbleFrame::set_margin(int margin)
{
	Q_UNUSED(margin);
}


void BubbleFrame::set_widget(QWidget* w)
{
	if (this->h_layout_m->count()>0)
		return;
	else
	{
		this->h_layout_m->addWidget(w);
	}
}


void BubbleFrame::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.setPen(Qt::white);
	if (this->role_m==ChatRole::OTHER)
	{
		//画气泡
		QColor bk_color(Qt::white);
		painter.setBrush(QBrush(bk_color));
		QRect bk_rect = QRect(WIDTH_SANJIAO, 0, this->width() - WIDTH_SANJIAO, this->height());
		painter.drawRoundedRect(bk_rect, 5, 5);
		//绘制三角形
		QPointF points[3]={
			QPointF(bk_rect.x(),12),
			QPointF(bk_rect.x(),10+WIDTH_SANJIAO+2),
			QPointF(bk_rect.x()-WIDTH_SANJIAO,10+WIDTH_SANJIAO-WIDTH_SANJIAO/2),
		};
		painter.drawPolygon(points, 3);//绘制
	}
	else
	{
		QColor bk_color(158, 234, 106);
		painter.setBrush(QBrush(bk_color));
		//画气泡
		QRect bk_rect = QRect(0, 0, this->width() - WIDTH_SANJIAO, this->height());
		painter.drawRoundedRect(bk_rect, 5, 5);
		//画三角
		QPointF points[3] = {
			QPointF(bk_rect.x() + bk_rect.width(), 12),
			QPointF(bk_rect.x() + bk_rect.width(), 12 + WIDTH_SANJIAO + 2),
			QPointF(bk_rect.x() + bk_rect.width() + WIDTH_SANJIAO, 10 + WIDTH_SANJIAO - WIDTH_SANJIAO / 2),
		};
		painter.drawPolygon(points, 3);
	}
	return QFrame::paintEvent(event);
}
