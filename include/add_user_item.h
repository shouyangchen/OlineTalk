#pragma once

#include <QWidget>
#include "ListItemBase.h"
#include "ui_add_user_item.h"

QT_BEGIN_NAMESPACE
namespace Ui { class add_user_itemClass; };
QT_END_NAMESPACE

class add_user_item : public ListItemBase
{
	Q_OBJECT

public:
	add_user_item(QWidget *parent = nullptr);
	~add_user_item();
	QSize sizeHint() const override
	{
		return QSize(250, 70);
	}
private:
	Ui::add_user_itemClass *ui;
};

