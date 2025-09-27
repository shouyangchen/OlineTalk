#ifndef MULTISELECTIONHISTORYWIDGET_H
#define MULTISELECTIONHISTORYWIDGET_H
#include <QWidget>
#include "ChatHistoryView.h"
class MultiSelectionHistoryWidget :public QWidget
{
	Q_OBJECT
private:
	friend  class ChatHistoryView;
public:
	MultiSelectionHistoryWidget(QWidget* parent = nullptr);
	~MultiSelectionHistoryWidget()override=default;
};
#endif

