#ifndef CONNECTUSERLISTVIEW_H
#define CONNECTUSERLISTVIEW_H
#include <QListView>
class ConnectUserListView : public QListView
{
	Q_OBJECT
public:
	ConnectUserListView(QWidget* parent = nullptr);
	~ConnectUserListView() override = default;
};
#endif
