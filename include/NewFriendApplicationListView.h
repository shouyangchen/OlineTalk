#ifndef NEWADDFRIENDAPPLICATIONLISTVIEW_H
#define NEWADDFRIENDAPPLICATIONLISTVIEW_H
#include <QListView>
class NewFriendApplicationListView : public QListView
{
	Q_OBJECT
public:
	explicit NewFriendApplicationListView(QWidget* parent = nullptr);
	~NewFriendApplicationListView() override;

};
#endif
