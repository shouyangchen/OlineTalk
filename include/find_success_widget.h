#ifndef FINDSUCCESSWIDGET_H
#define  FINDSUCCESSWIDGET_H

#include <QWidget>
#include <ui_find_success_widget.h>
#include "add_friend_widget.h"
#include <memory>
#include <QDialog>

#include "user_info_mgr.h"

class SearchList;

QT_BEGIN_NAMESPACE
namespace Ui { class find_success_widgetClass; };
QT_END_NAMESPACE

class find_success_widget : public QDialog
{
	friend class SearchList;
	friend class add_friend_widget;
	Q_OBJECT

public:
	find_success_widget(QWidget *parent = nullptr);
	void set_serach_info(std::shared_ptr<SearchInfo>si);
	~find_success_widget();
	signals:
		void will_close();

private:
	Ui::find_success_widgetClass *ui;
	std::shared_ptr<SearchInfo> search_info_m;

public slots:
	void on_the_add_friend_clicked();
};
#endif

