#include "PictureBubble.h"
#include <QLabel>
#define PIC_MAX_WIDTH 160
#define PIC_MAX_HEIGHT 90

PictureBubble::PictureBubble(const QPixmap& picture, ChatRole role, QWidget* parent):BubbleFrame(role,parent)
{
	//构造函数的解释:创建一个可点击的标签并设置其图片

	ClickedLabel* lb = new ClickedLabel();
	lb->setScaledContents(true);
	QPixmap pix = picture.scaled(QSize(PIC_MAX_WIDTH, PIC_MAX_HEIGHT), Qt::KeepAspectRatio);
	lb->setPixmap(pix);
	this->set_widget(lb);
	int left_margin = this->layout()->contentsMargins().left();
	int right_margin = this->layout()->contentsMargins().right();
	int v_margin = this->layout()->contentsMargins().bottom();
	this->setFixedSize(pix.width() + left_margin + right_margin, pix.height() + v_margin * 2);
}
