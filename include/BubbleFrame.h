#ifndef BUBBLE_FRAME_H
#define BUBBLE_FRAME_H
#include <QFrame>
#include <QHBoxLayout>
#include "global.h"

class BubbleFrame:public QFrame
{
	Q_OBJECT
public:
	BubbleFrame(ChatRole role, QWidget* parent = nullptr);
	void set_margin(int margin);
	void set_widget(QWidget* w);
protected:
	void paintEvent(QPaintEvent* event) override;
private:
	QHBoxLayout* h_layout_m;
	ChatRole role_m;
	int margin_m;
};
#endif

