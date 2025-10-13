#pragma once

#include <QWidget>
#include "ui_NewAddFriendApplicationWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class NewAddFriendApplicationWidgetClass; };
QT_END_NAMESPACE

class NewAddFriendApplicationWidget : public QWidget
{
	Q_OBJECT

public:
	NewAddFriendApplicationWidget(QWidget *parent = nullptr);
	~NewAddFriendApplicationWidget();

private:
	Ui::NewAddFriendApplicationWidgetClass *ui;
};

