#ifndef CHICKEDONCELABEL_H
#define CHICKEDONCELABEL_H
#include <QDialog>
#include <QLabel>

class ClickedOnceLabel:public QLabel
{
	Q_OBJECT
public:
	explicit ClickedOnceLabel(QWidget* parent = nullptr);
	~ClickedOnceLabel() override = default;
	signals:
		void clicked(QString text);
protected:
	void mouseReleaseEvent(QMouseEvent* event) override;
};
#endif
