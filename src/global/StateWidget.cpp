#include "StateWidget.h"

/*
 * 实现思路：
 * 我们设置一个widget作为状态栏的基础，然后在这个widget上我们可以放置一些标签来显示消息计数器等信息。
 * 使用两个标签：一个显示图标，一个显示消息计数器。
 * 将显示消息计数器的标签设置为不可见，只有当有消息时才显示。
 */
StateWidget::StateWidget(QWidget* parent)
	: QWidget(parent), the_message_count_m(0)
{
    // 创建两个独立的label
    this->display_the_message_icon_label_m = new DisplayMessageNumsLabel(this);
	this->display_the_message_icon_label_m->setObjectName("state_widget_message_icon_label");
    QHBoxLayout* h_box_layout = new QHBoxLayout();
    h_box_layout->setContentsMargins(0, 0, 0, 0);
    h_box_layout->setAlignment(Qt::AlignCenter);
    h_box_layout->setSpacing(2); // 小间距，让两个控件紧密相邻
    
    // 添加图标label和消息计数label
    h_box_layout->addWidget(this->display_the_message_icon_label_m, 0, Qt::AlignCenter);
    this->setLayout(h_box_layout);

}

StateWidget::~StateWidget()
{
}

bool StateWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (watched==this->display_the_message_icon_label_m&&event->type()==QEvent::DragLeave)
    {
	   /* this->display_the_message_icon_label_m->clear_message_nums();*/
    }
   return  QWidget::eventFilter(watched, event);
}

void StateWidget::increment_message_count()
{
    this->display_the_message_icon_label_m->increment_message_nums();
	qDebug() << "Message count incremented to:" << get_message_count();
}

void StateWidget::clear_message_count()
{
	this->the_message_count_m.store(0, std::memory_order_release);
	qDebug() << "Message count cleared";
}

void StateWidget::set_message_count(unsigned char count)
{
    this->the_message_count_m.store(count, std::memory_order_release);
	this->display_the_message_icon_label_m->set_message_nums(count);
}

void StateWidget::setPixmap(const QPixmap& pixmap)
{
    if (this->display_the_message_icon_label_m)
        this->display_the_message_icon_label_m->setPixmap(pixmap);
}

unsigned char StateWidget::get_message_count() const
{
    return the_message_count_m.load(std::memory_order_acquire);
}
