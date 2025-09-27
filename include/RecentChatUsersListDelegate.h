#ifndef RECENTCHATUSERSLISTDELEGATE_H
#define RECENTCHATUSERSLISTDELEGATE_H
#include <QStyledItemDelegate>
#include <QPainter>
#include <QApplication>
#include <QDateTime>
#include "RecentChatUsersListModel.h"
class RecentChatUsersListDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	RecentChatUsersListDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}
protected:
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

};
#endif