#ifndef CLICKEDFARM_H
#define CLICKEDFARM_H
#include <QFrame>
class ClickedFrame : public QFrame
{
	Q_OBJECT
public:
	ClickedFrame(QWidget* parent = nullptr);
	~ClickedFrame()override=default;
	signals:
		void sig_clicked(const bool&);
		void sig_clicked();//重载的点击信号(普通的点击信号)
protected:
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
};
#endif

