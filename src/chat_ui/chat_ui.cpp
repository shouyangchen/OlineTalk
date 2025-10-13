#include "chat_ui.h"
#include <QCloseEvent>
#include <QApplication>
#include <QTimer>
#include <QOverload>
#include "ui_chat_ui.h"
#include "SystemTrayIcon.h"
#include <QDebug>
#include <QMovie>
#include <QFile>
#include  "ConnectUserListDisplayDelegate.h"
#include "add_user_item.h"
#include "chat_user_display.h"
#include "loadinguserwidget.h"
#include "the_user_icon_mgr.h"
#include "user_info_mgr.h"
#include "StyleManager.h"
#include "RecentChatUsersList.h"
#include "RecentChatUsersListModel.h"

chat_ui::chat_ui(QWidget* parent)
    :QWidget(parent),mode(CHAT_UI_MODE::ChatMode),state(CHAT_UI_MODE::ChatMode),is_loading(false)
    , ui(new Ui::chat_ui)
{
    qDebug() << "Creating chat_ui instance";

    try {
        ui->setupUi(this);
        this->setMinimumHeight(600);
        this->setMinimumWidth(900);
        this->init_the_hash_of_stack_widget();
        this->ui->talk_view->setPixmap(
	        QPixmap(":/res/message.png").scaled(40, 40, Qt::KeepAspectRatioByExpanding,
	        Qt::SmoothTransformation));;
        this->ui->serach_edit->set_max_length(10);
        QAction* serach_action = new QAction(ui->serach_edit);
        serach_action->setIcon(QIcon(":/res/search.png"));
        this->ui->serach_edit->addAction(serach_action, QLineEdit::LeadingPosition);
        QAction* serach_clear_action = new QAction(this->ui->serach_edit);
        serach_clear_action->setIcon(QIcon(":/res/close_transparent.png"));//设置透明的清除图片
        connect(this->ui->serach_edit, &SerachEdit::textChanged, [serach_clear_action](const QString& text)
            {
                if (!text.isEmpty())
                    serach_clear_action->setIcon(QIcon(":/res/close_search.png"));
                else
                    serach_clear_action->setIcon(QIcon(":/res/close_transparent.png"));

            });

        connect(serach_clear_action, &QAction::triggered, [this, serach_clear_action]()
            {
                this->ui->serach_edit->clear();
                serach_clear_action->setIcon(QIcon(":/res/close_transparent.png"));
                this->ui->serach_edit->clearFocus();//清除焦点
                this->show_serach(false);
            });
        this->ui->new_friends_frame->setFrameStyle(~(this->ui->new_friends_frame->frameStyle()));
		this->ui->new_friends_frame->setVisible(false);//初始状态不显示新的好友申请控件
        this->ui->serach_edit->addAction(serach_clear_action, QLineEdit::TrailingPosition);
		this->show_serach(false);//初始状态不显示搜索
        this->setWindowIcon(QIcon(":/res/default_user_icon.png"));
        this->connect_sig();
        this->set_qss();
        this->installEventFilter(this);//设置事件过滤器
		this->ui->serach_edit->setFocusPolicy(Qt::ClickFocus);//设置为点击才获得焦点
        this->ui->user_icon->setPixmap(QPixmap(":/res/default_user_icon.png").scaled(40,40,Qt::KeepAspectRatio, Qt::SmoothTransformation));
        this->ui->about_view->setPixmap(QPixmap(":/res/settings_press.png").scaled(40, 40, Qt::KeepAspectRatio,Qt::SmoothTransformation));
        this->ui->about_view->setObjectName("about_view");
		this->ui->talk_view->setObjectName("talk_view");
        this->ui->connect_users->setPixmap(QPixmap(":/res/contact_list_press.png").scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		this->ui->about_view->setToolTip("设置");
		this->ui->talk_view->setToolTip("消息");
        
        // 先创建模型
        auto connectModel = new ConnectUserListModel();
        this->ui->connect_user_list->setModel(connectModel);
        this->ui->connect_user_list->setItemDelegate(new ConnectUserListDisplayDelegate());
        
        auto recentChatModel = new RecentChatListModel(this);
        this->ui->chat_user_list->setModel(recentChatModel);
        // 建立信号连接
        connect(recentChatModel, &RecentChatListModel::sig_request_users_name, 
                connectModel, &ConnectUserListModel::slot_get_users_name);
        connect(connectModel, &ConnectUserListModel::sig_connect_users_name_loading_done, 
                recentChatModel, &RecentChatListModel::slot_the_recent_user_name_loading_done);
        connect(connectModel, &ConnectUserListModel::sig_connect_user_name_changed, 
                recentChatModel, &RecentChatListModel::slot_the_user_name_changed);
        
        // 关键：当ConnectUserListModel数据准备好后，通知RecentChatListModel请求用户名
        connect(connectModel, &ConnectUserListModel::userListUpdated, 
                recentChatModel, &RecentChatListModel::loading_users_name);
        
        // 其他连接保持不变...
        
        recentChatModel->loading_user_icon_f();
        connect(this->ui->chat_user_list, &RecentChatUsersList::user_selected, 
                recentChatModel, &RecentChatListModel::slot_user_selected);
        auto message_label = this->ui->chat_side_bar->findChild<DisplayMessageNumsLabel*>("state_widget_message_icon_label");
        if (message_label) {
            connect(message_label, &DisplayMessageNumsLabel::sig_clear_all_unread_message, 
                    recentChatModel, &RecentChatListModel::slot_clear_all_unread_message);
            connect(recentChatModel, &RecentChatListModel::totalUnreadMessageCountChanged, 
                    message_label, &DisplayMessageNumsLabel::set_message_nums);
        }
        qDebug() << "chat_ui setup completed successfully";
    }
    catch (const std::exception& e) {
        qDebug() << "Error in chat_ui constructor:" << e.what();
        throw;
    }
    catch (...) {
        qDebug() << "Unknown error in chat_ui constructor";
        throw;
    }
}

void chat_ui::connect_sig()
{
    connect(SystemTrayIcon::getInstance()->TrayIcon_Actions[EXIT_APP], &QAction::triggered, this, &chat_ui::when_touch_close);
    connect(SystemTrayIcon::getInstance()->TrayIcon_Actions[OPEN_CHAT_UI], &QAction::triggered, this, &chat_ui::show);
    //connect(this->ui->chat_user_list, &ChatUserList::sig_loading_chat_user, this, &chat_ui::slot_loading_the_user_list);
    connect(this->ui->serach_edit, &SerachEdit::sig_focus_in, [this]()
        {
            //当获得焦点的时候显示搜索的QListWidget
            this->show_serach(true);
        });
    connect(this->ui->about_view->display_the_message_icon_label_m, &ClickedLabel::Clicked, [this]
        {
			this->ui->stackedWidget->setCurrentIndex(the_stack_widget_index->value("about_view"));
			this->ui->new_friends_frame->setVisible(false);//隐藏新的好友申请控件
			this->show_serach(false);

        });
    connect(this->ui->talk_view->display_the_message_icon_label_m, &ClickedLabel::Clicked, [this]
        {
			this->ui->stackedWidget->setCurrentIndex(the_stack_widget_index->value("chat_view"));
			this->show_serach(false);
            this->ui->chat_user_list->setVisible(true);
            this->ui->new_friends_frame->setVisible(false);//隐藏新的好友申请控件
        });
    connect(this->ui->chat_user_list, &RecentChatUsersList::doubleClicked, [this](const QModelIndex& index)
        {
			this->ui->stackedWidget->setCurrentIndex(the_stack_widget_index->value("chat_view"));//切换到聊天页面
			auto user_name = index.data(RecentChatListModel::UserNameRole).toString();//获取用户名
			auto user_id = index.data(RecentChatListModel::UserIdRole).toString();//获取用户id
            this->state = CHAT_UI_MODE::ChatMode;
            this->show_serach(false);
			//设置聊天页面的标题
			auto widget = dynamic_cast<ChatPage*>(this->ui->stackedWidget->currentWidget());//获取当前的聊天页面
			widget->ui->title_lb->setText(user_name);//设置标题
			auto chat_history_view = dynamic_cast<ChatHistoryView*>(widget->ui->chat_view->get_view());//获取聊天记录视图
			auto model = dynamic_cast<RecentChatListModel*>(this->ui->chat_user_list->model());//获取聊天用户列表的模型
			model->get_model_from_cache(user_id)->set_display_view(chat_history_view);//设置聊天记录视图
            model->clear_the_unread_message_count(index);
			model->get_model_from_cache(user_id)->setUserId(index.data(RecentChatListModel::UserIdRole).value<QString>());//设置用户id
			QPixmap user_icon = index.data(RecentChatListModel::UserAvatarRole).value<QPixmap>();//获取用户头像
            model->get_model_from_cache(user_id)->setUserIcon(user_icon);
			if (model)//设置聊天记录视图的模型
                chat_history_view->setModel(model->get_model_from_cache(user_id));
            model->get_model_from_cache(user_id)->loading_chat_history();
			chat_history_view->scrollToBottom();//滚动到最底部
			});
    connect(this->ui->connect_users->display_the_message_icon_label_m, &ClickedLabel::Clicked, [this]
        {
			this->ui->new_friends_frame->setVisible(true);//显示新的好友申请控件
            this->ui->connect_user_list->setVisible(true);
            this->ui->chat_user_list->setVisible(false);
        });
    using no_value_clicked_sig_ptr = void (ClickedFrame::*)();
    no_value_clicked_sig_ptr sig_ptr = &ClickedFrame::sig_clicked;

    connect(this->ui->new_friends_frame,sig_ptr, [this]
        {
    		this->ui->stackedWidget->setCurrentIndex(the_stack_widget_index->value("new_friend_applications_widget"));
			qDebug() << "New friend applications clicked";
        });

}

void chat_ui::closeEvent(QCloseEvent* event)
{
    if (event->spontaneous())
    {
        event->ignore();
        this->hide();
    }
    else
    {
        event->accept();
        QWidget::closeEvent(event);
        QApplication::quit();
    }
}

void chat_ui::add_user_list()
{

}


void chat_ui::init_the_hash_of_stack_widget()
{
    the_stack_widget_index = new QHash<QString, int>();
    the_stack_widget_index->insert("chat_view", 0);
    the_stack_widget_index->insert("setting_view", 1);
    the_stack_widget_index->insert("connect_view", 2);
    the_stack_widget_index->insert("about_view", 3);
	the_stack_widget_index->insert("new_friend_applications_widget", 4);
	the_stack_widget_index->insert("null_widget", 5);

}




chat_ui::~chat_ui()
{
    qDebug() << "Destroying chat_ui instance";

    try {
        disconnect();

        if (ui) {
            qDebug() << "Deleting chat_ui UI";
            delete ui;
            ui = nullptr;
        }
        if (the_stack_widget_index) {
            qDebug() << "Deleting the_stack_widget_index";
            delete the_stack_widget_index;
            the_stack_widget_index = nullptr;
		}
        qDebug() << "chat_ui destroyed successfully";
    }
    catch (const std::exception& e) {
        qDebug() << "Error in chat_ui destructor:" <<e.what();
    }
    catch (...) {
        qDebug() << "Unknown error in chat_ui destructor";
    }
}

void chat_ui::mouseMoveEvent(QMouseEvent* event)
{
    QPoint event_pos = event->globalPosition().toPoint();
    if (moving && (event->buttons() & Qt::LeftButton)
        && (event_pos - this->last_point__pos).manhattanLength() > QApplication::startDragDistance())
    {
        this->move(event_pos - this->last_point__pos);
        this->last_point__pos = event_pos - this->pos();
    }
    return QWidget::mouseMoveEvent(event);
}

void chat_ui::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        moving = true;
        this->last_point__pos = event->globalPosition().toPoint() - this->pos();
    }
    return QWidget::mousePressEvent(event);
}

void chat_ui::slot_loading_the_user_list()
{
	if (this->is_loading)
		return;
	this->is_loading = true;
    auto loading_widget = new LoadingUserWidget(this);
    loading_widget->show();
    add_user_list();
    loading_widget->deleteLater();
    this->is_loading = false;
}



void chat_ui::mouseReleaseEvent(QMouseEvent* event)
{
    this->moving = false;
    event->accept();
}

void chat_ui::when_touch_close()
{
    if (sender() == SystemTrayIcon::getInstance()->TrayIcon_Actions[EXIT_APP])
        this->close();
    else
        this->showMinimized();
}

void chat_ui::show_serach(bool is_serach)
{
	if (is_serach)
	{
        this->ui->chat_user_list->hide();
        this->ui->connect_user_list->hide();
        this->ui->serach_list->show();
        this->mode = CHAT_UI_MODE::SerachMode;
	}
    else if (this->state==CHAT_UI_MODE::ChatMode)
    {
        this->ui->chat_user_list->show();
        this->ui->connect_user_list->hide();
        this->ui->serach_list->hide();
        this->mode = CHAT_UI_MODE::ChatMode;
    }
    else if (this->state==CHAT_UI_MODE::ConnectMode)
    {
        this->ui->chat_user_list->hide();
        this->ui->serach_list->hide();
        this->ui->connect_user_list->show();
        this->mode = CHAT_UI_MODE::ConnectMode;
    }
}


void chat_ui::init_add_user_item()
{

}

bool chat_ui::eventFilter(QObject* watched, QEvent* event)
{
	if (watched==this&&event->type()==QEvent::MouseButtonPress)
	{
        auto mouse_press_event = static_cast<QMouseEvent*>(event);
		if (this->mode==CHAT_UI_MODE::SerachMode)//如果搜索列表显示
          {
              QPoint pos_in_serach_list = this->ui->serach_list->mapFromGlobal(mouse_press_event->globalPos());
              if (!this->ui->serach_list->rect().contains(pos_in_serach_list))
              {
                  this->ui->serach_edit->clear();
                  this->ui->serach_edit->clearFocus();
                  this->show_serach(false);
                  return QWidget::eventFilter(watched, event);
              }
          }
		else
            return QWidget::eventFilter(watched, event);
	}
    return QWidget::eventFilter(watched, event);
}


void chat_ui::set_qss()
{
    // 使用StyleManager加载QSS样式表
    if (!StyleManager::getInstance()->loadStyleSheet(this, ":/style/chat_ui.qss")) {
        qDebug() << "Failed to load chat UI stylesheet, falling back to default styles";
        // 如果无法加载QSS文件，保留一些基本样式作为后备
        this->setStyleSheet(
            "QWidget#chat_ui { background-color: #F8F9FA; }"
            "QWidget#chat_side_bar { background-color: #5A6C7D; border: none; }"
        );
    }

    // 设置搜索框的占位符文本
    ui->serach_edit->setPlaceholderText("搜索好友");
}
