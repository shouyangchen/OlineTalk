#include "NewAddFriendApplicationWidget.h"

#include "NewFriendApplicationListModel.h"

NewAddFriendApplicationWidget::NewAddFriendApplicationWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::NewAddFriendApplicationWidgetClass())
{
	ui->setupUi(this);
}

NewAddFriendApplicationWidget::~NewAddFriendApplicationWidget()
{
	delete ui;
}

