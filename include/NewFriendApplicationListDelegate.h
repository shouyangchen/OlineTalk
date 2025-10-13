#ifndef NEWADDFRIENDAPPLICATIONDELEGATE_H
#define NEWADDFRIENDAPPLICATIONDELEGATE_H
#include <QStyledItemDelegate>
class NewFriendApplicationListDelegate :public  QStyledItemDelegate
{
	Q_OBJECT
public:
	explicit NewFriendApplicationListDelegate(QObject* parent = nullptr);
	~NewFriendApplicationListDelegate() override = default;
protected:
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};
#endif

