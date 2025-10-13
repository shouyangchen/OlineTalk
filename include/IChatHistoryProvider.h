#ifndef	ICHATE_HISTORY_PROVIDER_H
#define ICHATE_HISTORY_PROVIDER_H
#include <QList>
#include "global.h"

class IChatHistoryProvider :public QObject
{
	Q_OBJECT
public:
    IChatHistoryProvider() = default;
    virtual ~IChatHistoryProvider() = default;

    // 获取历史聊天记录
    virtual QList<ChatMessage> getHistory(const QString& userId, int offset, int count) = 0;

    // 增加新的聊天记录
    virtual bool addMessage(const QString& userId, const ChatMessage& message) = 0;

    // 获取总数
    virtual int getHistoryCount(const QString& userId) = 0;

	virtual QList<QString> getUserIdFromDataBase() = 0;// 获取所有有聊天记录的用户ID列表
};




#endif
