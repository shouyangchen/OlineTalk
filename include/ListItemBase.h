#ifndef LISTITEMBASE_H
#define LISTITEMBASE_H
#include "global.h"
#include <QWidget>



class ListItemBase:public QWidget
{
public:
	explicit ListItemBase(QWidget*parent=nullptr);
	void set_item_type(ListItemType itemType);
	ListItemType get_item_type();
private:
	ListItemType item_type;
};
#endif
