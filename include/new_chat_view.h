#ifndef NEW_CHAT_VIEW_H
#define NEW_CHAT_VIEW_H
#include <QWidget>
#include <QVBoxLayout>
#include "ChatHistoryDataModel.h"
#include "ChatHistoryView.h"
class New_ChatView : public QWidget
{

	Q_OBJECT
private:
	ChatHistoryView* chatHistoryView;
	friend class ChatHistoryDataModel;
	friend class ChatPage;
	std::atomic<bool> loading_done;
	void set_chat_history_view();
public:
	New_ChatView(QWidget* parent = nullptr);
	~New_ChatView()override = default;
	QAbstractItemModel* get_model();
	QListView* get_view() { return this->chatHistoryView; }
protected:
	bool eventFilter(QObject* watched, QEvent* event) override;
};
#endif
