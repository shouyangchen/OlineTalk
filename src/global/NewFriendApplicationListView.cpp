#include "NewFriendApplicationListView.h"
#include "NewFriendApplicationListDelegate.h"
#include "NewFriendApplicationListModel.h"

NewFriendApplicationListView::NewFriendApplicationListView(QWidget* parent):QListView(parent)
{
	this->setItemDelegate(new NewFriendApplicationListDelegate(this));
	QListView::setModel(new NewFriendApplicationListModel());
	this->setSelectionRectVisible(false);//选中的背影不可见
	this->setEditTriggers(EditTrigger::NoEditTriggers);//不可编辑
}

NewFriendApplicationListView::~NewFriendApplicationListView()
{
	delete this->model();
}