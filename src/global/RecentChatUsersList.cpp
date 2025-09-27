#include "RecentChatUsersList.h"
#include "RecentChatUsersListModel.h"
#include "RecentChatUsersListDelegate.h"
#include <QDebug>
RecentChatUsersList::RecentChatUsersList(QWidget* parent)
	: QListView(parent)
{
	auto model= new RecentChatListModel(this);
	setItemDelegate(new RecentChatUsersListDelegate(this));
	setModel(model);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	// Bridge: clicked -> user_selected(QString) -> model->slot_user_selected(QString)
	connect(this, &QListView::clicked, this, [this](const QModelIndex& index) {
		if (!index.isValid())
			return;
		QString user_id = index.data(RecentChatListModel::UserIdRole).toString();
		emit user_selected(user_id);
	});
	connect(this, &RecentChatUsersList::user_selected, model, &RecentChatListModel::slot_user_selected);
}
