#include "chat_ui.h"
#include <QCloseEvent>
#include <QApplication>
#include <QMouseEvent>
#include "ui_chat_ui.h"
#include "SystemTrayIcon.h"
#include <QDebug>
#include <QAction>

#include "chat_user_display.h"

chat_ui::chat_ui(QWidget* parent)
    :QWidget(parent),mode(CHAT_UI_MODE::ChatMode),state(CHAT_UI_MODE::ChatMode),is_loading(false)
    , ui(new Ui::chat_ui)
{
    qDebug() << "Creating chat_ui instance";

    try {
        ui->setupUi(this);
        this->setMinimumHeight(600);
        this->setMinimumWidth(900);

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
        qDebug() << "Title text set to:" << ui->title_lb->text();
        qDebug() << "Title label size:" << ui->title_lb->size();
        qDebug() << "Title widget size:" << ui->title_widget->size();

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
    // 整体窗口样式 - 与 QSS 完全一致
    this->setStyleSheet(
        "QWidget#chat_ui {"
        "    background-color: #F8F9FA;"
        "}"
    );

    // 侧边栏样式 - 使用 QSS 中的蓝灰色主题
    ui->chat_side_bar->setStyleSheet(
        "QWidget#chat_side_bar {"
        "    background-color: #5A6C7D;"
        "    border: none;"
        "}"
    );

    // 侧边栏图标样式 - 与 QSS 风格一致，确保图标显示
    QString sidebarLabelStyle =
        "QLabel {"
        "    background-color: transparent;"
        "    border-radius: 8px;"
        "    padding: 8px;"
        "    margin: 3px;"
        "    color: #D5D8DC;"
        "    font-size: 14px;"
        "    min-width: 40px;"
        "    min-height: 40px;"
        "    max-width: 40px;"
        "    max-height: 40px;"
        "}"
        "QLabel:hover {"
        "    background-color: #4A5B6B;"
        "    color: #FFFFFF;"
        "}";

    ui->user_icon->setStyleSheet(sidebarLabelStyle);
    ui->talk_view->setStyleSheet(sidebarLabelStyle);
    ui->setting_view->setStyleSheet(sidebarLabelStyle);
    ui->about_view->setStyleSheet(sidebarLabelStyle);


    // 用户列表区域样式 - 与 QSS 保持一致
    ui->chat_user->setStyleSheet(
        "QWidget#chat_user {"
        "    background-color: #FFFFFF;"
        "    border-right: 1px solid #D5D8DC;"
        "}"
    );

    // 搜索区域样式 - 与 QSS 保持一致
    ui->serach_widget->setStyleSheet(
        "QWidget#serach_widget {"
        "    background-color: #FFFFFF;"
        "    border-bottom: 1px solid #D5D8DC;"
        "    padding: 10px;"
        "}"
    );

    // 搜索框样式 - 完全按照 QSS 输入框样式
    ui->serach_edit->setStyleSheet(
        "QLineEdit {"
        "    border: 1px solid #D5D8DC;"
        "    border-radius: 8px;"
        "    padding: 10px 14px;"
        "    background: #FFFFFF;"
        "    font-size: 14px;"
        "    color: #2C3E50;"
        "}"
        "QLineEdit:focus {"
        "    border: 2px solid #7B8FA3;"
        "    background: #FFFFFF;"
        "    outline: none;"
        "}"
        "QLineEdit::placeholder {"
        "    color: #8B9DC3;"
        "}"
    );
    ui->serach_edit->setPlaceholderText("搜索好友");

    // 添加好友按钮样式 - 使用 QSS 主按钮色彩
    ui->add_button->setStyleSheet(
        "StatusChangeButton {"
        "    background-color: #5A6C7D;"
        "    border: none;"
        "    border-radius: 8px;"
        "    padding: 6px;"
        "}"
        "StatusChangeButton:hover {"
        "    background-color: #4A5B6B;"
        "}"
        "StatusChangeButton:pressed {"
        "    background-color: #3D4A56;"
        "}"
    );

    // 用户列表样式 - 与 QSS 保持一致的颜色
// 用户列表样式 - 修改为匹配 Chat_User_Display 组件
    QString listWidgetStyle =
        "QListWidget {"
        "    background-color: #FFFFFF;"
        "    border: none;"
        "    outline: none;"
        "    font-size: 14px;"
        "    padding: 0px;"
        "    spacing: 0px;"
        "}"
        "QListWidget::item {"
        "    padding: 0px;"
        "    margin: 0px;"
        "    border: none;"
        "    background-color: transparent;"
        "    min-height: 50px;"
        "    max-height: 50px;"
        "    border-bottom: 1px solid #E5E5EA;"
        "}"
        "QListWidget::item:selected {"
        "    background-color: #E3F2FD;"
        "    border-bottom: 1px solid #E5E5EA;"
        "}"
        "QListWidget::item:hover {"
        "    background-color: #F8F9FA;"
        "}";

    ui->serach_list->setStyleSheet(listWidgetStyle);
    ui->chat_user_list->setStyleSheet(listWidgetStyle);
    ui->connect_user_list->setStyleSheet(listWidgetStyle);
    // 聊天主区域样式 - 与 QSS 背景保持一致
    ui->chat_ui_data->setStyleSheet(
        "QWidget#chat_ui_data {"
        "    background-color: #F8F9FA;"
        "}"
    );

    // 标题区域样式 - 与 QSS 保持一致
    ui->title_widget->setStyleSheet(
        "QWidget#title_widget {"
        "    background-color: #FFFFFF;"
        "    border-bottom: 1px solid #D5D8DC;"
        "}"
    );

    // 标题标签样式 - 修复对齐问题
    ui->title_lb->setStyleSheet(
        "QLabel#title_lb {"
        "    color: #1A252F;"
        "    font-size: 16px;"
        "    font-weight: 600;"
        "    padding: 12px 16px;"
        "    background-color: transparent;"
        "    border: none;"
        "    margin: 0px;"
        "    qproperty-alignment: 'AlignLeft | AlignVCenter';"
        "}"
    );

    // 手动设置标题对齐
    ui->title_lb->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // 工具栏样式 - 与 QSS 保持一致
    ui->tool_widget->setStyleSheet(
        "QWidget#tool_widget {"
        "    background-color: #FFFFFF;"
        "    border-bottom: 1px solid #D5D8DC;"
        "}"
    );

    // 工具栏图标样式 - 与 QSS 按钮风格一致
    QString toolLabelStyle =
        "QLabel {"
        "    background-color: transparent;"
        "    border-radius: 8px;"
        "    padding: 8px;"
        "    margin: 4px;"
        "    font-size: 16px;"
        "    color: #5A6C7D;"
        "}"
        "QLabel:hover {"
        "    background-color: #F8F9FA;"
        "    color: #4A5B6B;"
        "}";

    ui->emjo_lb->setStyleSheet(toolLabelStyle);
    ui->file_lb->setStyleSheet(toolLabelStyle);

    // 聊天数据区域样式 - 与 QSS 保持一致
    ui->chat_data_list->setStyleSheet(
        "QWidget#chat_data_list {"
        "    background-color: #FFFFFF;"
        "    border-top: 1px solid #D5D8DC;"
        "    padding: 8px;"
        "}"
    );

    // 文本编辑框样式 - 完全按照 QSS 输入框样式
    ui->textEdit->setStyleSheet(
        "QTextEdit {"
        "    border: 1px solid #D5D8DC;"
        "    border-radius: 8px;"
        "    padding: 10px 14px;"
        "    background: #FFFFFF;"
        "    font-size: 14px;"
        "    color: #2C3E50;"
        "    selection-background-color: #F0F2F5;"
        "    line-height: 1.4;"
        "}"
        "QTextEdit:focus {"
        "    border: 2px solid #7B8FA3;"
        "    background: #FFFFFF;"
        "    outline: none;"
        "}"
        // 滚动条样式 - 完全按照 QSS
        "QScrollBar:vertical {"
        "    background-color: #F8F9FA;"
        "    width: 8px;"
        "    border-radius: 4px;"
        "    margin: 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background-color: #C5D0DB;"
        "    border-radius: 4px;"
        "    min-height: 20px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "    background-color: #A8B8C8;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "    height: 0px;"
        "}"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
        "    background: transparent;"
        "}"
    );

    // 底部按钮区域样式 - 与 QSS 保持一致
    ui->widget_5->setStyleSheet(
        "QWidget#widget_5 {"
        "    background-color: #FFFFFF;"
        "    border-top: 1px solid #D5D8DC;"
        "    padding: 10px;"
        "}"
    );

    // 接收按钮样式 - 修复显示问题
    ui->QP_recived->setStyleSheet(
        "QPushButton#QP_recived {"
        "    background-color: #FFFFFF;"
        "    color: #5A6C7D;"
        "    border: 1px solid #C5D0DB;"
        "    border-radius: 8px;"
        "    padding: 8px 16px;"
        "    font-size: 14px;"
        "    font-weight: 500;"
        "    text-align: center;"
        "}"
        "QPushButton#QP_recived:hover {"
        "    background-color: #F8F9FA;"
        "    color: #4A5B6B;"
        "    border-color: #A8B8C8;"
        "}"
        "QPushButton#QP_recived:pressed {"
        "    background-color: #F0F2F5;"
        "    color: #3D4A56;"
        "    border-color: #95A5B5;"
        "}"
    );

    // 发送按钮样式
    ui->QP_send->setStyleSheet(
        "QPushButton#QP_send {"
        "    background-color: #5A6C7D;"
        "    color: #FFFFFF;"
        "    border: none;"
        "    border-radius: 8px;"
        "    padding: 8px 16px;"
        "    font-size: 14px;"
        "    font-weight: 500;"
        "    text-align: center;"
        "}"
        "QPushButton#QP_send:hover {"
        "    background-color: #4A5B6B;"
        "}"
        "QPushButton#QP_send:pressed {"
        "    background-color: #3D4A56;"
        "}"
        "QPushButton#QP_send:disabled {"
        "    background-color: #C5D0DB;"
        "    color: #8B9DC3;"
        "}"
    );

    // 手动设置按钮尺寸确保文字显示
    ui->QP_recived->setMinimumSize(80, 32);
    ui->QP_recived->setMaximumSize(QWIDGETSIZE_MAX, 32);
    ui->QP_recived->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    ui->QP_send->setMinimumSize(80, 32);
    ui->QP_send->setMaximumSize(QWIDGETSIZE_MAX, 32);
    ui->QP_send->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}
