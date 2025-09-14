#include "user_icon_frame.h"

#include <qevent.h>

#include "loginui.h"

user_icon_frame::user_icon_frame(QWidget* parent) :QFrame(parent)
{}

user_icon_frame::~user_icon_frame()
{
	
}


void user_icon_frame::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    int width = this->width();
    int height = this->height();
    QPoint center(width / 2, height / 2);
    int radius = qMin(width, height) / 2 - 2;
    if (!this->user_icon.isNull()) {
        // 缩放图片为圆的直径大小
        QPixmap scaledPixmap = this->user_icon.scaled(radius * 2, radius * 2, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // 计算图片左上角，使其居中
        int x = center.x() - scaledPixmap.width() / 2;
        int y = center.y() - scaledPixmap.height() / 2;

        // 创建圆形裁剪区域
        QPainterPath path;
        path.addEllipse(center, radius, radius);
        painter.setClipPath(path);

        painter.drawPixmap(x, y, scaledPixmap);
        event->accept();
    }
    else {
		// 图片加载失败或者未设置，绘制默认QFrame背景
		return QFrame::paintEvent(event); // 调用基类的绘制方法

    }
}


void user_icon_frame::draw_user_icon(const QPixmap& user_icon)
{
 this->user_icon = user_icon;
 this->update(); // 更新界面以重新绘制
}