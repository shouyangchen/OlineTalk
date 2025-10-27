#include "serach_list.h"
#include <QDebug>
#include <QMouseEvent>
#include <QApplication>

#include "add_user_item.h"
#include "TcpMgr.h"
#include "the_user_icon_mgr.h"

SearchList::SearchList(QWidget* parent):QListWidget(parent),find_w(nullptr),send_pending(false)
{
	Q_UNUSED(parent)
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//初始状态不显示滚动条
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//安装事件过滤器
	this->viewport()->installEventFilter(this);
	connect(this, &QListWidget::itemClicked, this, &SearchList::on_the_item_clicked);
	this->add_tip_item();
	connect(TcpMgr::getInstance().get(), &TcpMgr::sig_user_serach, this, &SearchList::slot_user_seracher);
	// Safely obtain search_edit: parent may be null or UI not yet built. Defer fallback lookup to click handler.
	this->search_edit = nullptr;
	if (parent) {
		// try direct child first
		SerachEdit* edit = parent->findChild<SerachEdit*>("search_edit");
		if (!edit) {
			// try top-level window
			QWidget* win = parent->window();
			if (win && win != parent) edit = win->findChild<SerachEdit*>("search_edit");
		}
		this->search_edit = edit;
	}

}

bool SearchList::eventFilter(QObject* watched, QEvent* event)
{
	if (watched==this->viewport())
	{
		if (event->type() == QEvent::Enter)
		{//当鼠标进入时，显示垂直滚动条
			this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		}
		if (event->type()==QEvent::Leave)
		{
			this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		}
	}
	if (watched==this->viewport()&&event->type()==QEvent::Wheel)
	{
		QWheelEvent* wheel_event = static_cast<QWheelEvent*>(event);
		int num_degrees = wheel_event->angleDelta().y() / 8;//每次滚动的角度
		int num_steps = num_degrees / 15;//每步15度
		//滚动滚动条
		this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - num_steps * this->verticalScrollBar()->singleStep());
		return true;//事件处理完成
	}
	return QListWidget::eventFilter(watched, event);
}


void SearchList::add_tip_item()
{
	auto* invalid_item = new QWidget();
	QListWidgetItem* item_tmp = new QListWidgetItem;
	//qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
	item_tmp->setSizeHint(QSize(250, 10));
	this->addItem(item_tmp);
	invalid_item->setObjectName("invalid_item");
	this->setItemWidget(item_tmp, invalid_item);
	item_tmp->setFlags(item_tmp->flags() & ~Qt::ItemIsSelectable);


	auto* add_item_tmp = new add_user_item();
	QListWidgetItem* item = new QListWidgetItem;
	//qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
	item->setSizeHint(add_item_tmp->sizeHint());
	this->addItem(item);
	this->setItemWidget(item, add_item_tmp);
}

void SearchList::on_the_item_clicked(QListWidgetItem* item)
{
	QWidget* item_wid = this->itemWidget(item);
	if (!item_wid)
	{
		return;
	}
	else
	{
		ListItemBase* base = dynamic_cast<ListItemBase*>(item_wid);
		if (base&&base->get_item_type()==ListItemType::ADD_USER_ITEM)
		{
			if (send_pending)
			{
				return;
			}
			this->wait_pending(true);
			// Ensure we have a valid SerachEdit pointer (UI may not have been ready at ctor time)
			SerachEdit* searchEditPtr = dynamic_cast<SerachEdit*>(this->search_edit);
			if (!searchEditPtr) {
				// try to find dynamically on click
				QWidget* win = this->parentWidget() ? this->parentWidget()->window() : QApplication::activeWindow();
				if (win) {
					searchEditPtr = win->findChild<SerachEdit*>("serach_edit");
				}
			}
			if (!searchEditPtr) {
				qDebug() << "SearchList: SerachEdit not found, aborting search request.";
				this->wait_pending(false);
				return;
			}
			auto uid_str = searchEditPtr->text();
			if (uid_str.isEmpty())
				return;//搜索内容为空则不处理
			QJsonObject json_object;
			if (adjust_is_email(uid_str))//如果搜索关键字为email
			{
				json_object["keyword"] = "email";// keyword只能为 'uid', 'username', 'email'
			} else {
				json_object["keyword"] = "uid";//否者默认为uid搜索
			}
			json_object["search_value"] = uid_str;//搜索的值
			QJsonDocument doc(json_object);
			TcpMgr::getInstance()->send_data(Message_id::SEARCH_USER, doc.toJson());
			qDebug() << "发送搜索用户请求！";
			if (!this->find_w)
			{
				this->find_w = new find_success_widget(this->parentWidget());
				this->find_w->setAttribute(Qt::WA_DeleteOnClose);
				connect(this->find_w, &find_success_widget::will_close, this, &SearchList::close_find_widget);
				//this->find_w->show();当数据接收到并且解析好之后才显示
			}
			 return;
		}
	}
	
}

void SearchList::set_search_edit(QWidget* edit)
{

}

void SearchList::close_find_widget()
{
	if (find_w)
	{
		find_w->close();
		find_w = nullptr;//释放资源	
	}
}


void SearchList::wait_pending(bool pending)
{
	
}

void SearchList::slot_user_seracher(std::vector<std::shared_ptr<SearchInfo>> user_list)
{
	if (!this->find_w) {
		// defensive: create if not present
		this->find_w = new find_success_widget(this->parentWidget());
		this->find_w->setAttribute(Qt::WA_DeleteOnClose);
		connect(this->find_w, &find_success_widget::will_close, this, &SearchList::close_find_widget);
	}
	this->find_w->search_info_m = user_list.front();
	this->find_w->ui->friend_name->setText(user_list.front()->name);
	QPixmap pixmap;
	pixmap.loadFromData(QByteArray::fromBase64(user_list.front()->icon.toUtf8()));
	the_user_icon_mgr::getInstance()->add_user_icon(user_list.front()->uid, QString {}, pixmap);
	this->find_w->ui->friend_icon->setPixmap(pixmap.scaled(this->find_w->ui->friend_icon->width(), this->find_w->ui->friend_icon->height(), Qt::KeepAspectRatio));
	this->find_w->show();
}
SearchList::~SearchList() = default;