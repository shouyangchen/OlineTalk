#ifndef STATUSCHANGEBUTTON_H
#define STATUSCHANGEBUTTON_H
#include <QPushButton>
class StatusChangeButton:public QPushButton
{
	Q_OBJECT
public:
	explicit StatusChangeButton(QWidget* parent = nullptr);
	explicit StatusChangeButton(const QString& text, QWidget* parent = nullptr);
	StatusChangeButton(const QIcon& icon, const QString& text, QWidget* parent = nullptr);
	~StatusChangeButton();
protected:
	void enterEvent(QEnterEvent* event) override;
	void leaveEvent(QEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
};
#endif
