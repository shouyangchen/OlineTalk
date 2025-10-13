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
protected:
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
};
#endif

