#ifndef CONNECTUSERLISTDISPLAYDELEGATE_H
#define CONNECTUSERLISTDISPLAYDELEGATE_H
#include <QStyledItemDelegate>
#include <QPainter>
#include <QModelIndex>
class ConnectUserListDisplayDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	explicit ConnectUserListDisplayDelegate(QObject* parent = nullptr);
	~ConnectUserListDisplayDelegate() override = default;
protected:
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};
#endif
