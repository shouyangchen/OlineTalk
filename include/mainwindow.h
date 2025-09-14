//
// Created by chenshouyang on 25-6-23.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "loginui.h"
#include <QSystemTrayIcon>
#include "SystemTrayIcon.h"
#include <QThread>

// 前向声明，避免循环包含
class chat_ui;

QT_BEGIN_NAMESPACE
namespace Ui { class mainwindow; }
QT_END_NAMESPACE

class mainwindow : public QMainWindow {
Q_OBJECT

public:
    explicit mainwindow(QWidget *parent = nullptr);
    ~mainwindow() override;
public slots:
    void display_chat_ui();
private:
    Ui::mainwindow *ui;
    loginui *loginui_instance;//登陆界面实例
    chat_ui* chat_ui_instance; // 初始化为nullptr
    QThread* work_thread;
    void connect_signals();
};

#endif //MAINWINDOW_H
