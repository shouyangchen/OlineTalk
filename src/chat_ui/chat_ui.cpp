#include "chat_ui.h"
#include <QCloseEvent>
#include <QApplication>
#include <QMouseEvent>
#include "ui_chat_ui.h"
#include "SystemTrayIcon.h"
#include <QDebug>
#include <QAction>
#include <QMovie>
#include <QFile>

#include "chat_user_display.h"
#include "loadinguserwidget.h"
#include "the_user_icon_mgr.h"
#include "user_info_mgr.h"
#include "StyleManager.h"

chat_ui::chat_ui(QWidget* parent)
    :QWidget(parent),mode(CHAT_UI_MODE::ChatMode),state(CHAT_UI_MODE::ChatMode),is_loading(false)
    , ui(new Ui::chat_ui)
{
    qDebug() << "Creating chat_ui instance";

    try {
        ui->setupUi(this);
        this->setMinimumHeight(600);
        this->setMinimumWidth(900);

        this->ui->talk_view->setPixmap(
	        QPixmap(":/res/msg_chat_normal.png").scaled(40, 40, Qt::KeepAspectRatioByExpanding,
	        Qt::SmoothTransformation));
   //     auto user_icon = the_user_icon_mgr::getInstance()->get_user_icon(
			//QString::number(user_info_mgr::getInstance()->get_user_id()));
		//if (!user_icon.isNull())
		//	this->ui->user_icon->setPixmap(user_icon.scaled(50, 50, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
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

        this->ui->serach_edit->addAction(serach_clear_action, QLineEdit::TrailingPosition);
		this->show_serach(false);//初始状态不显示搜索
        this->setWindowIcon(QIcon(":/res/default_user_icon.png"));
        this->connect_sig();
        this->set_qss();
        this->add_user_list();
        // 调试输出
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

std::vector<QString>msg={
	"今天天气真好！",
	"你最近在忙什么？",
	"周末有空一起出去玩吧！",
	"你喜欢看电影吗？",
	"工作压力大吗？",
	"最近有什么好书推荐吗？",
	"你喜欢什么类型的音乐？",
	"假期打算去哪里旅游？",
	"你喜欢运动吗？",
	"最近有什么有趣的事情发生吗？"
};

std::vector<QString>head={
	":/res/head_1.jpg",
	":/res/head_2.jpg",
	":/res/head_3.jpg",
	":/res/head_4.jpg",
	":/res/head_5.jpg",
};


std::vector<QString>names={
	"小王",
	"小李",
	"小郑",
	"小赵",
	"小刘",
	"小陈",
	"小杨",
	"小孙",
};
void chat_ui::connect_sig()
{
    connect(SystemTrayIcon::getInstance()->TrayIcon_Actions[EXIT_APP], &QAction::triggered, this, &chat_ui::when_touch_close);
    connect(SystemTrayIcon::getInstance()->TrayIcon_Actions[OPEN_CHAT_UI], &QAction::triggered, this, &chat_ui::show);
    connect(this->ui->chat_user_list, &ChatUserList::sig_loading_chat_user, this, &chat_ui::slot_loading_the_user_list);
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
	for (int i=0;i<13;i++)
	{
        int random_value = QRandomGenerator::global()->bounded(100);//生成99之间的随机数
		int str_i = random_value % msg.size();
		int head_i = random_value % head.size();
        int name_i = random_value % names.size();
        Chat_User_Display* item = new Chat_User_Display();
        item->set_info(names[name_i], head[head_i], msg[str_i]);
        QListWidgetItem* listItem = new QListWidgetItem();
        listItem->setSizeHint(item->sizeHint());
        this->ui->chat_user_list->addItem(listItem);
		this->ui->chat_user_list->setItemWidget(listItem, item);
	}
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
        qDebug() << "chat_ui destroyed successfully";
    }
    catch (const std::exception& e) {
        qDebug() << "Error in chat_ui destructor:" << e.what();
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
