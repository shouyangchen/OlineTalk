#ifndef CHAT_HISTORY_VIEW_H
#define CHAT_HISTORY_VIEW_H
#include <QListView>
#include <QAbstractItemModel>
#include "ChatHistoryDataModel.h"
class ChatHistoryView:public QListView
{
	Q_OBJECT
private:
	friend class ChatHistoryDataModel;
	friend class ChatPage;
	bool need_loading;
	std::atomic<bool>loading_done;
	QString user_id;
	QList<QAction*>* Actions_list_m;
	QMenu* the_menu;
	QModelIndex click_index;
	void init_actions();
	void connect_the_action_sig();
public:
	ChatHistoryView(QWidget* parent = nullptr);
	~ChatHistoryView()override;
public slots:
	void slot_replace_the_model(QAbstractItemModel* model);
	void slot_set_user_id(QString const& user_id);
protected:
	bool eventFilter(QObject* watched, QEvent* event) override;
};
#endif

