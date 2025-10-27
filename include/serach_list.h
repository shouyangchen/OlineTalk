#ifndef SERACH_LIST_H
#define SERACH_LIST_H
#include <QListWidget>
#include <QWheelEvent>
#include <QScrollBar>
#include "SerachEdit.h"
#include "user_info_mgr.h"
#include<QWidget>

#include "find_success_widget.h"


class SearchList:public QListWidget
{
	Q_OBJECT
private:
	void wait_pending(bool pending = false);
	bool send_pending;
	void add_tip_item();
	find_success_widget* find_w;
	QWidget* search_edit;
public:
	SearchList(QWidget* parent = nullptr);
	void set_search_edit(QWidget* edit);
	void close_find_widget();
	~SearchList()override;
protected:
	bool eventFilter(QObject* watched, QEvent* event) override;
public slots:
	void on_the_item_clicked(QListWidgetItem* item);
	void slot_user_seracher(std::vector<std::shared_ptr<SearchInfo>> user_info);
};
#endif

