#ifndef CHATHISTORYITEMSELECTIONMODEL_H
#define CHATHISTORYITEMSELECTIONMODEL_H
#include <QItemSelectionModel>
#include "ChatHistoryView.h"
#include "ChatHistoryDataModel.h"
#include <QMenu>
#include "singleton.h"
class ChatHistoryItemSelectionModel:public QItemSelectionModel,public singleton<ChatHistoryItemSelectionModel>
{
	Q_OBJECT
private:
	friend class singleton<ChatHistoryItemSelectionModel>;
	friend class ChatHistoryView;
	friend class ChatHistoryDataModel;
	explicit ChatHistoryItemSelectionModel(QAbstractItemModel* model = nullptr);
	explicit ChatHistoryItemSelectionModel(QAbstractItemModel* model, QObject* parent);
public:
	ChatHistoryItemSelectionModel(ChatHistoryItemSelectionModel&&) = delete;
	ChatHistoryItemSelectionModel(ChatHistoryItemSelectionModel&) = delete;
	ChatHistoryItemSelectionModel& operator=(ChatHistoryItemSelectionModel&) = delete;
	~ChatHistoryItemSelectionModel()override;
	QAbstractItemModel* get_the_item_model();
public slots:
	void slot_replace_the_mode(QAbstractItemModel* model);//切换所跟踪的model
};





#endif