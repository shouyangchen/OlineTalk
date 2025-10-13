#ifndef DISPLAYUSERICONSLABEL_H
#define DISPLAYUSERICONSLABEL_H
#include <QLabel>
class DisplayUserIconsLabel : public QLabel
{
	Q_OBJECT
public:
	 explicit DisplayUserIconsLabel(QWidget* parent = nullptr);
signals:
	void sig_display_the_user_icon(QPixmap const&);
protected:
	void mouseReleaseEvent(QMouseEvent* event) override;
};
#endif