#ifndef DISPLAYMESSAGENUMSLABEL_H
#define DISPLAYMESSAGENUMSLABEL_H
#include <QLabel>
#include <QPointF>
#include <QMouseEvent>
#include <atomic>
#include "ClickedLabel.h"
class DisplayMessageNumsLabel : public ClickedLabel
{
	Q_OBJECT
private:
	std::atomic<unsigned char>the_message_nums_m;
	QPointF drag_start_position_m;
	QRect painter_rect;
	int diameter;
public:
	DisplayMessageNumsLabel(QWidget* parent = nullptr);
	~DisplayMessageNumsLabel()override = default;
	void increment_message_nums();//用于增加消息计数器并且更新标签显示
	void set_message_nums(unsigned char count);//用于直接设置消息计数器值
	unsigned char get_message_nums() const;//用于获取当前的消息计数器值
	signals:
		void sig_increment_message_nums();
		void sig_clear_all_unread_message();
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	QPixmap get_notify_pixmap();
public slots:
	void clear_message_nums();//用于清零消息计数器并且更新标签显示
	void slot_increment_message_nums();
};
#endif
