#ifndef USER_ICON_FRAME_H
#define USER_ICON_FRAME_H
#include <QPainter>
#include <QFrame>
#include <QPainterPath>

class user_icon_frame  : public QFrame
{
	Q_OBJECT
private:
	QPixmap user_icon;
public:
	user_icon_frame(QWidget *parent);
	virtual ~user_icon_frame();
public slots:
	void draw_user_icon(const QPixmap& user_icon);
protected:
	void paintEvent(QPaintEvent*) override;
};
#endif // USER_ICON_FRAME_H

