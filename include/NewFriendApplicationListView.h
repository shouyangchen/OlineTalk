#ifndef NEWADDFRIENDAPPLICATIONLISTVIEW_H
#define NEWADDFRIENDAPPLICATIONLISTVIEW_H
#include "global.h"
#include <QListView>
#include <QAction>
class NewFriendApplicationListView : public QListView
{
	Q_OBJECT
private:
	QAction* accept_action;
	QAction* reject_action;
public:
	explicit NewFriendApplicationListView(QWidget* parent = nullptr);
	~NewFriendApplicationListView() override;
signals:
	void sig_accept_application_no_recent_chat_list(const the_connected_user_info& );//同意好友申请通过该信号通知最近联系人列表和好友列表更新
	void sig_accept_application_on_connect_user_list(const connectUserList::user_info&);//同意好友申请通过该信号通知联系人列表更新
protected:
	bool eventFilter(QObject* watched, QEvent* event) override;
 public slots:
	void acceptApplication();
	void rejectApplication();
};
#endif
