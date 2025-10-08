#include "ChatHistoryView.h"
#include <QWheelEvent>
#include "ChatHistoryItemSelectionModel.h"
#include <QEvent>
#include <QScrollBar>
#include <QTimer>
#include <QClipboard>
#include "ChatHistoryDataModel.h"
#include "ChatHistoryDisplayDelegate.h"
#include "chatpage.h"


ChatHistoryView::ChatHistoryView(QWidget* parent) :QListView(parent), need_loading(true),user_id("12345")
{
	this->viewport()->installEventFilter(this);
	this->Actions_list_m = new QList<QAction*>();
	this->the_menu = new QMenu(this);
	this->the_menu->setObjectName("the_function_menu");
	this->init_actions();
	this->connect_the_action_sig();
}

bool ChatHistoryView::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == this->viewport() && event->type() == QEvent::Wheel)
	{
		QWheelEvent* wheel_event = dynamic_cast<QWheelEvent*>(event);

		// 判断向上滚动
		if (wheel_event->angleDelta().y() > 0)
		{
			if (this->model()->rowCount()==0)
			{
				dynamic_cast<ChatHistoryDataModel*>(this->model())->loading_chat_history();
				return  QListView::eventFilter(watched, event);
			}
			// 判断是否到最上方
			if (this->verticalScrollBar()->value() == this->verticalScrollBar()->minimum())
			{
				// 此处可以加载更多历史消息
				auto model = this->model();
				dynamic_cast<ChatHistoryDataModel*>(model)->loading_befor_chat_history();
				QTimer::singleShot(500, this, [this]() { this->need_loading = true; });
			}
		}
	}
	if (watched==this->viewport()&&event->type()==QEvent::MouseButtonPress)
	{
		//如果是右键且且选中了某一项或者是在某一项上右键那么就显示右键菜单
		QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
		if (mouse_event->button()==Qt::RightButton)
		{
			auto press_pos = mouse_event->pos();
			this->click_index = this->indexAt(press_pos);
			if (click_index.isValid())
			{
				//在某一项上右键然后显示右键菜单或者是显示一个widget用来显示右键菜单
				qDebug() <<"行数"<<click_index.row();
				this->update(click_index);
				this->the_menu->exec(this->viewport()->mapToGlobal(press_pos));
			}
			else
				return QListView::eventFilter(watched, event);
		}
	}
	return QListView::eventFilter(watched, event);
}


void ChatHistoryView::slot_replace_the_model(QAbstractItemModel* model)
{
	auto model_past = this->model();
	model_past->disconnect();
	this->setModel(model);
	this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
	if (!model_past)
		return;
	model_past->deleteLater();
	dynamic_cast<ChatHistoryDataModel*>(model)->loading_chat_history(this->user_id);
}


void ChatHistoryView::slot_set_user_id(QString const& user_id)
{
	this->user_id = user_id;
}


void ChatHistoryView::init_actions()
{
	QStringList action_names = { u8"删除",u8"复制",u8"转发",u8"收藏",u8"多选" };
	for (auto &item:action_names)
	{
		auto action = new QAction();
		action->setText(item);
		this->Actions_list_m->append(action);
	}
	this->the_menu->addActions(this->Actions_list_m->toVector());
}


void ChatHistoryView::connect_the_action_sig()
{
	for (auto item:*this->Actions_list_m)
	{
		if (item->text() == u8"复制" && item)
			connect(item, &QAction::triggered, [this](const bool clicked)
				{
					Q_UNUSED(clicked)
					QClipboard* clipboard = QGuiApplication::clipboard();
					auto data_from_model = this->click_index.data(Qt::DisplayRole);
					clipboard->setText(data_from_model.toString());
					qDebug() << "复制成功";
				});

		if (item->text()==u8"多选"&&item)
			connect(item, &QAction::triggered, [this](const bool clicked)
				{
					this->setSelectionMode(QAbstractItemView::MultiSelection);
					auto parent_parent_widget =dynamic_cast<ChatPage*> (this->parentWidget()->parentWidget()->parentWidget());//ChatDataPage
					qDebug() << parent_parent_widget->metaObject()->className();
					qDebug() << "parent_widget" << parent_parent_widget;
					if (!parent_parent_widget)
						return;
					parent_parent_widget->ui->tool_widget->setVisible(false);
					parent_parent_widget->ui->QP_recived->setVisible(false);
					parent_parent_widget->ui->QP_send->setVisible(false);
					parent_parent_widget->ui->MultiSelectionHistoryWidget_widget->setVisible(true);
					parent_parent_widget->ui->textEdit->setVisible(false);
					qDebug() << "多选成功";
				});
	}
}

ChatHistoryView::~ChatHistoryView()
{
	for (auto& item :*this->Actions_list_m)
		delete item;
	this->Actions_list_m->clear();
	delete this->Actions_list_m;
}

