#include "ChatHistoryDataModel.h"
#include <QListView>
ChatHistoryDataModel::ChatHistoryDataModel(QObject* parent):QAbstractListModel(parent),offset(0)
{
	this->chatHistoryView = nullptr;
	connect(this, &ChatHistoryDataModel::sig_send_the_message, this, &ChatHistoryDataModel::slot_send_the_message);
}

ChatHistoryDataModel::ChatHistoryDataModel(ChatHistoryView*view):QAbstractListModel(nullptr),offset(0)
{
	this->chatHistoryView = view;
	connect(this, &ChatHistoryDataModel::sig_send_the_message, this, &ChatHistoryDataModel::slot_send_the_message);
}

int ChatHistoryDataModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;
	return chatMessages.size();
}

QVariant ChatHistoryDataModel::data(const QModelIndex& index, int role) const
{
	//否是无效索引
	//实现数据的获取
	//role是数据的角色
	//Qt::DisplayRole 显示数据Qt::EditRole 编辑数据
	//Qt::UserRole 用户自定义数据

	if (!index.isValid() || index.row() < 0 || index.row() >= chatMessages.size())
		return QVariant();
	const ChatMessage& message = chatMessages.at(index.row());
	if (role == Qt::DisplayRole)
	{
		return message.message_context;
	}
	else if (role == Qt::UserRole)
	{
		return message.timestamp;
	}
	else if (role == Qt::UserRole + 2)
	{
		return message.message_type;
	}
	else if (role==Qt::UserRole+3)
	{
		return message.sender;//SELF or OTHER
	}
	return QVariant();
}

void ChatHistoryDataModel::clearData()
{
	beginResetModel();
	chatMessages.clear();
	endResetModel();
}

void ChatHistoryDataModel::insertMessages(int insertPos, const QList<ChatMessage>& messages)
{
	//插入数据，参数：插入位置，插入的数据，
	//注意线程安全问题
	if (messages.isEmpty())
		return;
	beginInsertRows(QModelIndex(), insertPos, insertPos + messages.size() - 1);
	for (int i = 0; i < messages.size(); ++i)
		chatMessages.insert(insertPos, messages[i]);
	endInsertRows();
	this->chatHistoryView->loading_done.store(true, std::memory_order_release);
}

void ChatHistoryDataModel::appendMessages(const QList<ChatMessage>& messages)
{
	//追加数据，参数：插入位置，插入的数据，
	//注意线程安全问题
	if (messages.isEmpty())
		return;
	int insertPos = chatMessages.size();
	beginInsertRows(QModelIndex(), insertPos, insertPos + messages.size() - 1);
	for (const auto& msg : messages)
		chatMessages.append(msg);
	endInsertRows();
	HistoryDB_Mgr::getInstance()->slot_addMessage(messages[0].sender, messages);
}

void ChatHistoryDataModel::setUserIcon(const QPixmap& icon)
{
	 this->user_icon = icon; 
}

void ChatHistoryDataModel::slot_send_the_message(const ChatMessage& messages, QString const& uid)
{
	this->appendMessages({ messages });
	this->chatHistoryView->scrollToBottom();
	HistoryDB_Mgr::getInstance()->addMessage(uid, messages);
}


void ChatHistoryDataModel::loading_chat_history(const QString& uid)
{
	this->offset += this->rowCount();//每次加载10条数据偏移量增加而且偏移量由当前的数据量决定
	auto message_list=HistoryDB_Mgr::getInstance()->getHistory(uid, offset, 6);
	if (!message_list.isEmpty())
		this->appendMessages(message_list);
}


void ChatHistoryDataModel::loading_befor_chat_history(const QString& uid)
{
	//加载更早的聊天记录先拿到当前最早的时间戳
	auto earliestTimestamp = this->chatMessages.first().timestamp;
	auto message_list = HistoryDB_Mgr::getInstance()->get_more_earlier_history(uid, earliestTimestamp, 20);
	if (!message_list.isEmpty())
		this->insertMessages(0, message_list);
}
