#ifndef TIMERBUTTON_H
#define TIMERBUTTON_H

#include <QObject>
#include <QPushButton>
#include <QTimer>
#include "registerui.h"
class TimerButton:public QPushButton
{
    Q_OBJECT
    QTimer* timer;
    int second;
    friend  class registerui;
	friend  class forgetpasswdui;
public:
    TimerButton(QObject*parent=nullptr);
    TimerButton()=default;
    virtual ~TimerButton();
protected:
    void mouseReleaseEvent(QMouseEvent *e) override;


};

#endif // TIMERBUTTON_H
