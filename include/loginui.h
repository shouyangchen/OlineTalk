//
// Created by chenshouyang on 25-6-23.
//

#ifndef LOGINUI_H
#define LOGINUI_H
#include <QDialog>
#include "registerui.h"
#include <QMap>
#include "ClickedLabel.h"
#include "chat_ui.h"
#include "the_user_icon_mgr.h"
#include <QtSql/QtSql>
#include <QtSql/QSqlDatabase>
#include "forgetpasswdui.h"

QT_BEGIN_NAMESPACE
namespace Ui { class loginui; }
QT_END_NAMESPACE

class loginui : public QDialog {
Q_OBJECT

public:
    explicit loginui(QWidget *parent = nullptr);
    ~loginui() override;
    signals:
        void have_the_user_icon(QPixmap& user_icon);
public slots:
	void on_pushButton_login_clicked();// 登录按钮点击事件
	void do_when_login_mod_finish(ReqId req_id, QString res, ErrorCodes err);// 处理登录模块的HTTP响应
	void do_when_the_account_input_done();// 账号输入完成后先设置默认头像;
	QPixmap& get_user_icon() { return this->user_icon; }
private:
    Ui::loginui *ui;
	forgetpasswdui* forget_passwd_ui = nullptr;
    inline void set_the_passwd_input_line();
    void connect_signals();
    void init_handlers();
	QMap<ReqId, std::function<void(const QJsonObject&)>>handlers; //处理函数map
    void set_the_wrong_tip(const QString& str);
    void do_get_the_user_icon();
    QPixmap user_icon;
};


#endif //LOGINUI_H
