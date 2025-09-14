#include "ListItemBase.h"
ListItemBase::ListItemBase(QWidget* parent):QWidget(parent)
{
	
}

void ListItemBase::set_item_type(ListItemType itemType)
{
	this->item_type = itemType;
}


ListItemType ListItemBase::get_item_type()
{
	return this->item_type;
}
