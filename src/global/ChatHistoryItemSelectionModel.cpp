#include "ChatHistoryItemSelectionModel.h"
ChatHistoryItemSelectionModel::ChatHistoryItemSelectionModel(QAbstractItemModel* model):QItemSelectionModel(model)
{

}

ChatHistoryItemSelectionModel::ChatHistoryItemSelectionModel(QAbstractItemModel* model, QObject* parent):QItemSelectionModel(model, parent)
{

}




void ChatHistoryItemSelectionModel::slot_replace_the_mode(QAbstractItemModel* model)
{
	this->setModel(model);
}

QAbstractItemModel* ChatHistoryItemSelectionModel::get_the_item_model()
{
	return this->model();
}

ChatHistoryItemSelectionModel::~ChatHistoryItemSelectionModel()
{

}
