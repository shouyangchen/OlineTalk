#pragma once

#include <QWidget>
#include "ui_loadinguserwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LoadingUserWidgetClass; };
QT_END_NAMESPACE

class LoadingUserWidget : public QWidget
{
	Q_OBJECT
		friend class chat_ui;
public:
	LoadingUserWidget(QWidget *parent = nullptr);
	~LoadingUserWidget();

private:
	Ui::LoadingUserWidgetClass *ui;
};

