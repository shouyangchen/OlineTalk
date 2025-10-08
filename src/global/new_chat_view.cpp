#include "new_chat_view.h"
#include <QEvent>
#include <QWheelEvent>
#include "ChatHistoryItemSelectionModel.h"
#include "ChatHistoryDisplayDelegate.h"

New_ChatView::New_ChatView(QWidget* parent) :QWidget(parent), loading_done(false)
{
	this->chatHistoryView = new ChatHistoryView();
	auto v_layout =new QVBoxLayout();
	v_layout->setContentsMargins(0, 0, 0, 0);//设置布局内部间隙
	v_layout->addWidget(this->chatHistoryView);
	this->setLayout(v_layout);
	this->set_chat_history_view();
}

bool New_ChatView::eventFilter(QObject* watched, QEvent* event)
{

	return QWidget::eventFilter(watched, event);
}

void New_ChatView::set_chat_history_view()
{
	this->chatHistoryView->setModel(new ChatHistoryDataModel( this->chatHistoryView));
	this->chatHistoryView->setItemDelegate(new ChatHistoryDisplayDelegate());
	this->chatHistoryView->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
	this->chatHistoryView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);//设置垂直滚动条为按需显示
	this->chatHistoryView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);//设置垂直滚动模式为逐像素滚动
	this->chatHistoryView->setResizeMode(QListView::Adjust);//调整视图大小
	this->chatHistoryView->setStyleSheet("QListView::item:selected, QListView::item:hover { background: transparent; }");
	this->chatHistoryView->setSpacing(8);
	auto model = this->chatHistoryView->model();
	this->chatHistoryView->setSelectionModel(ChatHistoryItemSelectionModel::getInstance(model).get());
	this->chatHistoryView->setSelectionRectVisible(true);
	this->chatHistoryView->setSelectionMode(QAbstractItemView::NoSelection);//先设置为不可选中,当右键呼出菜单选择多选再设置为可选中
}

QAbstractItemModel* New_ChatView::get_model()
{
	return this->chatHistoryView->model();
}
