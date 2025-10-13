#include"DisplayUserIconsLabel.h"

DisplayUserIconsLabel::DisplayUserIconsLabel(QWidget* parent):QLabel(parent)
{
	
}

void DisplayUserIconsLabel::mouseReleaseEvent(QMouseEvent* event)
{
	QLabel::mouseReleaseEvent(event);//先调用父类的事件处理函数
	emit sig_display_the_user_icon(this->pixmap().scaled(200,
		200, Qt::AspectRatioMode::KeepAspectRatio, Qt::TransformationMode::SmoothTransformation));//发射信号，传递当前标签的图像，缩放到200x200
}