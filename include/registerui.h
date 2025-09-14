//
// Created by chenshouyang on 25-6-23.
//
//this file is part of the project "Qt-Login-Register-UI".

#ifndef REGISTERUI_H
#define REGISTERUI_H

#include <QDialog>
#include <QCloseEvent>
#include <QJsonObject>
#include "mistakekmgr.h"
#include "global.h"
#include "timerbutton.h"
#include <QMap>
QT_BEGIN_NAMESPACE
namespace Ui { class registerui; }
QT_END_NAMESPACE

class registerui : public QDialog {
Q_OBJECT
public:
	friend class TimerButton;
    explicit registerui(QWidget *parent = nullptr);
    ~registerui() override;

public slots:
    void on_pushButton_get_varify_code();
    void do_when_reg_mod_finish(ReqId req_id, QString res, ErrorCodes err);
    

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_pushButton_ok_clicked();

private:
    Ui::registerui *ui;
    MistakeMgr* mistake_mgr;
    void connect_signals();
    void set_the_passwd_input_line();
    void showTip(QString str,bool isok);
    void init_http_handlers();
    QMap<ReqId,std::function<void(const QJsonObject&)>> _handlers; // 用于存储处理函数的映射根据不同的请求id执行不同的逻辑
};


#endif //REGISTERUI_H
