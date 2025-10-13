#ifndef ADD_FRIEND_WIDGET_H
#define ADD_FRIEND_WIDGET_H

#include <QWidget>
#include "ui_add_friend_widget.h"
#include "global.h"
#include "user_info_mgr.h"

QT_BEGIN_NAMESPACE
namespace Ui { class add_friend_widgetClass; };
QT_END_NAMESPACE

class add_friend_widget : public QWidget
{
	Q_OBJECT

public:
	add_friend_widget(QWidget *parent = nullptr);
	void set_search_info(std::shared_ptr<SearchInfo> si);
	~add_friend_widget();

private:
	Ui::add_friend_widgetClass *ui;
	std::shared_ptr<SearchInfo> search_info_m;
	void connect_sig();
};
#endif

