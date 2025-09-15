#include "loadinguserwidget.h"

LoadingUserWidget::LoadingUserWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::LoadingUserWidgetClass())
{
	ui->setupUi(this);
	this->setAttribute(Qt::WA_TranslucentBackground, true);
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
	this->setFixedSize(this->parentWidget()->size());
}

LoadingUserWidget::~LoadingUserWidget()
{
	delete ui;
}

