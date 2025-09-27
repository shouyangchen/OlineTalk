#ifndef RECENTCHATUSERSLIST_H
#define RECENTCHATUSERSLIST_H
#include <QListView>

class RecentChatUsersList : public QListView
{
	Q_OBJECT
public:
	RecentChatUsersList(QWidget* parent = nullptr);
	~RecentChatUsersList() override=default;
signals:
	void user_selected(QString const& user_id);
};



#endif
