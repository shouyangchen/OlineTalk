#ifndef CHAT_HISTORY_DATA_MODEL_H
#define CHAT_HISTORY_DATA_MODEL_H
#include <mutex>
#include <QAbstractListModel>
#include "ChatHistoryView.h"
#include "global.h"
#include "HistoryDB_Mgr.h"

class ChatHistoryDataModel :public QAbstractListModel
{
	Q_OBJECT
private:
	friend  class ChatHistoryView;
	ChatHistoryView* chatHistoryView;
	QList<ChatMessage> chatMessages;
	QPixmap user_icon;
	QString user_id;
	int offset;
public:
	ChatHistoryDataModel(QObject* parent = nullptr);
	ChatHistoryDataModel(ChatHistoryView* view=nullptr);
	~ChatHistoryDataModel()override = default;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QString getUserID() { return this->user_id; }
	void clearData();
	void set_display_view(ChatHistoryView* view) { this->chatHistoryView = view; }
	void insertMessages(int insertPos, const QList<ChatMessage>& messages);
	void appendMessages(const QList<ChatMessage>& messages);
	void setUserIcon(const QPixmap& icon);
	void setUserId(const QString& uid) { this->user_id = uid; }
	void loading_chat_history(const QString& uid);
	void loading_chat_history();
	void loading_befor_chat_history(const QString& uid);
	void loading_befor_chat_history();
	signals:
	void sig_send_the_message(const ChatMessage& messages,QString const&uid);
public slots:
	void slot_send_the_message(const ChatMessage& messages, QString const& uid);
	void slot_received_message(const ChatMessage& message, QString const& uid);
};
#endif

