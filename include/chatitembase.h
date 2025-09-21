#ifndef CHAT_ITEM_BASE_H
#define CHAT_ITEM_BASE_H
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include "global.h"
class BubbleFrame;

class ChatItemBase:public QWidget
{
	Q_OBJECT
public:
	explicit ChatItemBase(ChatRole role, QWidget* parent = nullptr);
	void set_user_name(QString& name);
	void set_user_icon(QPixmap user_icon);
	void set_widget(QWidget* w);
private:
	ChatRole role_m;
	QLabel* name_label_m;
	QLabel* icon_label_m;
	QWidget* bubble_ml;
};
#endif
