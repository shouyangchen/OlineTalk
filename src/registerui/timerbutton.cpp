#include "timerbutton.h"
#include <QMouseEvent>
#include "registerui.h"
#include "forgetpasswdui.h"

TimerButton::TimerButton(QObject*parent) {
    this->timer=new QTimer (this);
    this->second=60;
    connect(this->timer,&QTimer::timeout,this,[this]()
            {
        if(this->second<=0)
        {
            this->timer->stop();
            this->setText("获取验证码");
            this->setEnabled(true);
            return;
        }
        else
            this->setText(QString("剩余%1秒").arg(this->second--));
    });
}


TimerButton::~TimerButton()
{
    if(this->timer->isActive())
        this->timer->stop();
}

void TimerButton::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton)
    {
        if (dynamic_cast<registerui*>(this->parentWidget())) {
            auto parent = dynamic_cast<registerui*>(this->parentWidget());
            if (parent->mistake_mgr->get_mistake_nums() == 0)
            {
                this->timer->start(1000);
                this->setEnabled(false);
                emit clicked();
                return QPushButton::mouseMoveEvent(e);
            }
				parent->showTip("请先修正错误！",false);
                return QPushButton::mouseMoveEvent(e);
        }
         if  (dynamic_cast<forgetpasswdui*>(this->parentWidget())) {
            auto parent = dynamic_cast<forgetpasswdui*>(this->parentWidget());
            if (parent->mistake_mgr->get_mistake_nums() == 0)
            {
                this->timer->start(1000);
                this->setEnabled(false);
                emit clicked();
                return QPushButton::mouseMoveEvent(e);
            }
        	    parent->showTip("请先修正错误！", false);
                return QPushButton::mouseMoveEvent(e);
		}
   
    }
    return QPushButton::mouseMoveEvent(e);
}
