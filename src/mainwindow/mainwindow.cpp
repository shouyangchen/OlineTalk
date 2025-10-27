//
// Created by chenshouyang on 25-6-23.
//

// You may need to build the project (run Qt uic code generator) to get "ui_loginui.h" resolved

#include "mainwindow.h"
#include <qlayout.h>
#include <ui_chat_ui.h>
#include <user_info_mgr.h>
#include "TcpMgr.h"
#include "ui_mainwindow.h"

mainwindow::mainwindow(QWidget* parent) :
    QMainWindow(parent), ui(new Ui::mainwindow) {
    ui->setupUi(this);
    this->work_thread = new QThread(this);


    TcpMgr::getInstance();
    this->work_thread->start();
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowMaximizeButtonHint);
    this->setWindowTitle("登录界面");
    this->loginui_instance = new loginui(); 
    this->setMaximumSize(QSize(350, 550));
    this->setMinimumSize(QSize(350, 550));
    this->setCentralWidget(this->loginui_instance);
    this->loginui_instance->show();//显示登录界面
    this->chat_ui_instance = new chat_ui();
    connect_signals();
    SystemTrayIcon::getInstance(QIcon(":/res/default_user_icon_withe_black.png"), nullptr);
    SystemTrayIcon::getInstance()->show();
    this->chat_ui_instance->show();
}


void mainwindow::display_chat_ui()
{
    qDebug() << "Attempting to display chat UI...";
    
    try {
        this->setMinimumWidth(600);
        this->setMinimumHeight(400);
        
        if (this->loginui_instance) {
            this->loginui_instance->hide();
        }
        SystemTrayIcon::getInstance()->setIcon(QIcon(":/res/default_user_icon.png"));
        this->hide();
        this->chat_ui_instance->ui->user_icon->setPixmap(
            the_user_icon_mgr::getInstance()->get_user_icon(
                QString::number(user_info_mgr::getInstance(QPixmap{},QString{}, QString{},
                    std::uint64_t{})->get_user_id()), QString{}));
		this->chat_ui_instance->get_friend_requests_list();//获取好友申请列表
        this->chat_ui_instance->show();
        
        qDebug() << "Chat UI displayed successfully";
    } catch (const std::exception& e) {
        qDebug() << "Error displaying chat UI:" << e.what();
    }
    
}

void mainwindow::connect_signals()
{
    try {
        auto tcpMgr = TcpMgr::getInstance();
        if (tcpMgr) {
            connect(tcpMgr.get(), &TcpMgr::sig_swtich_chat_dialog, this, &mainwindow::display_chat_ui);
            qDebug() << "Signals connected successfully";
        } else {
            qDebug() << "Failed to get TcpMgr instance";
        }
    } catch (const std::exception& e) {
        qDebug() << "Error connecting signals:" << e.what();
    }
    connect(this->loginui_instance, &loginui::have_the_user_icon, this->chat_ui_instance, &chat_ui::slot_replace_user_icon);
}

mainwindow::~mainwindow() {
    qDebug() << "Destroying mainwindow...";
    
    try {
        if (this->work_thread && this->work_thread->isRunning()) {
            this->work_thread->quit();
            this->work_thread->wait(3000); // Wait up to 3 seconds
        }
        
        delete ui;
        delete this->chat_ui_instance;
        qDebug() << "Mainwindow destroyed successfully";
    } catch (const std::exception& e) {
        qDebug() << "Error in mainwindow destructor:" << e.what();
    }
}
