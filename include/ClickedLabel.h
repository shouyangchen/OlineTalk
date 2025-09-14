#ifndef CLICKEDLABLE_H
#define CLICKEDLABLE_H

#include <QObject>
#include <QLabel>

class ClickedLabel:public QLabel
{
    Q_OBJECT
public:
    explicit ClickedLabel(QObject*parent=nullptr);
    virtual ~ClickedLabel();
signals:
    void  Clicked();
protected:
	void mousePressEvent(QMouseEvent* event) override;
};

#endif // CLICKEDLABLE_H
