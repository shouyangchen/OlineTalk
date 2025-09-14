//
// Created by chenshouyang on 25-6-23.
//

// You may need to build the project (run Qt uic code generator) to get "ui_registerui.h" resolved

#include "registerui.h"
#include "global.h"
#include "HttpMgr.h"
#include "loginui.h"
#include "ui_registerui.h"
#include "mistakekmgr.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QCryptographicHash>


registerui::registerui(QWidget *parent) :
    QDialog(parent), ui(new Ui::registerui) {
    ui->setupUi(this);
    this->mistake_mgr=new MistakeMgr(this);
    this->setAttribute(Qt::WA_DeleteOnClose); // Automatically delete the dialog when closed
    this->setWindowTitle("注册界面");
    this->set_the_passwd_input_line(); // Set the password input line to password mode
    this->ui->label_mistakenote->setProperty("state", "success"); //将错误提示标签设置为成功状态
    repolish_widget_func(this->ui->label_mistakenote); //刷新QSS
    this->connect_signals();
	this->init_http_handlers(); // 初始化HTTP处理函数
}

void registerui::closeEvent(QCloseEvent *event) {//重写关闭事件
    auto parent = this->parentWidget();
    if (parent) {
        parent->show(); //显示父界面
        parent->parentWidget()->show();
    }
    event->accept(); // Accept the close event
}

void registerui::connect_signals() {//连接信号和槽
    connect(this->ui->pushButton_cancle, &QPushButton::clicked, this, [this]() {
        this->close(); //关闭注册界面
        });
    connect(HttpMgr::getInstance().get(), &HttpMgr::reg_mod_finish, this, &registerui::do_when_reg_mod_finish);
	connect(this->ui->pushButton_ok, &QPushButton::clicked, this, &registerui::on_pushButton_ok_clicked);
    connect(this->ui->pushButton_get_verifiycation_code_at_registerui, &QPushButton::clicked, this, &registerui::on_pushButton_get_varify_code);
    connect(this->ui->lineEdit_the_email_at_registerui, &InputLine::editingFinished, this, [this]
        {
            auto email = this->ui->lineEdit_the_email_at_registerui->text();
            QRegularExpression regex(R"([A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,})");// 正则表达式匹配邮箱格式
            auto math = regex.match(email).hasMatch();// 检查邮箱格式是否正确
            if (math) {
                this->mistake_mgr->delete_mistake(ErrorCodes::EmailStlyeNotRight);
                this->showTip(tr(""), true);
            }
            return;
        });
    connect(this->ui->lineEdit_username_at_registerui, &InputLine::editingFinished, this, [this]
        {
            if (this->ui->lineEdit_username_at_registerui->text().isEmpty())
            {
                this->showTip(tr("用户名不能为空！"), false);
                return;
            }
            this->mistake_mgr->delete_mistake(ErrorCodes::UserNameIsNull);
            this->showTip(tr(""), true);
            return;
		});
    connect(this->ui->lineEdit_passwd_at_registerui, &InputLine::editingFinished, this, [this]
        {
            if (this->ui->lineEdit_passwd_at_registerui->text().isEmpty())
            {
                this->showTip(tr("密码不能为空！"), false);
                return;
            }
            this->mistake_mgr->delete_mistake(ErrorCodes::UserPasswdIsNull);
            this->showTip(tr(""), true);
            return;
        });
    connect(this->ui->lineEdit_verify_passwd_at_registerui, &InputLine::editingFinished, this, [this]
        {
            if (this->ui->lineEdit_verify_passwd_at_registerui->text().isEmpty())
            {
                this->showTip(tr("请再次输入密码！"), false);
                return;
            }
            if (this->ui->lineEdit_verify_passwd_at_registerui->text() != this->ui->lineEdit_passwd_at_registerui->text())
            {
                this->showTip(tr("两次输入的密码不一致！"), false);
                return;
            }
            this->mistake_mgr->delete_mistake(ErrorCodes::UserPasswdError);
            this->showTip(tr(""), true);
            return;
		});
}

void registerui::set_the_passwd_input_line() {//设置密码输入框为密码模式
    this->ui->lineEdit_passwd_at_registerui->setEchoMode(QLineEdit::Password);
    this->ui->lineEdit_verify_passwd_at_registerui->setEchoMode(QLineEdit::Password);
}


void registerui::on_pushButton_get_varify_code() {
    auto email = this->ui->lineEdit_the_email_at_registerui->text();
    QRegularExpression regex(R"([A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,})");// 正则表达式匹配邮箱格式
    auto math=regex.match(email).hasMatch();// 检查邮箱格式是否正确
    if (math) {
        // 发送验证码逻辑
        //this->ui->label_mistakenote->setText("验证码已发送到您的邮箱");
        //this->ui->label_mistakenote->setProperty("state", "success");
        this->mistake_mgr->delete_mistake(ErrorCodes::ERR_NETWORK);
		QJsonObject jsonObject;
        jsonObject["email"] = email;
		emit HttpMgr::getInstance()->sendRequest(QUrl(gate_url_prefix+"/get_varify_code"), jsonObject
            , ReqId::ID_GET_VARIFY_CODE, Modules::MODULE_REGISTER);
    } else {
       this->showTip(tr("请输入正确的邮箱地址！"),false);
    }
}

void registerui::showTip(QString str, bool isok) {
    if (isok) {
        this->ui->label_mistakenote->setProperty("state", "success");
    } else {
        this->ui->label_mistakenote->setProperty("state", "error");
    }
    this->ui->label_mistakenote->setText(str);
    repolish_widget_func(this->ui->label_mistakenote); //刷新QSS
}

void registerui::do_when_reg_mod_finish(ReqId req_id, QString res, ErrorCodes err) {
    if (err!=ErrorCodes::SUCCESS) {
        showTip(tr("网络错误！"),false);
        return;
    }
    //解析JSON响应，res是服务器返回的JSON字符串需要转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    if (jsonDoc.isNull()) {
        showTip(tr("JSON解析错误！"),false);

        return;
    }
    if (!jsonDoc.isObject()) {
        showTip(tr("JSON解析错误"),false);
        return;
    }
    QJsonObject jsonObj = jsonDoc.object();
    _handlers[req_id](jsonObj);

}

void registerui::init_http_handlers() {
    // 初始化HTTP处理函数映射
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE,[this](QJsonObject obj) {
       int error=obj["error"].toInt();
        if (error != ErrorCodes::SUCCESS) {
            showTip("参数错误！", false);
            return;
        }
        auto email = obj["email"].toString();
        showTip(tr("验证码已发送到您的邮箱：%1").arg(email), true);
        qDebug()<<email;
    });
    _handlers.insert(ReqId::ID_REG_USER, [this](QJsonObject obj) {
        auto error = obj["error"].toInt();
        if (error != ErrorCodes::SUCCESS) {
            showTip(tr("注册失败！请检查您的输入是否正确。"), false);
            return;
        }
        if (error==ErrorCodes::EmailIsRegister)
        {
            showTip(tr("该邮箱已被注册，请更换邮箱后重试！"), false);
			return;
        }
        auto email = obj["email"].toString();
        showTip("用户注册成功！", true);
        auto uid = obj["uid"].toInt();
        QString tip = tr("恭喜你注册成功！你的id为%1届时你可以通过绑定的邮箱号或者id号进行登录").arg(uid);
		QMessageBox::information(this, tr("注册成功"), tip);
        qDebug() << "User registered successfully with email:" << email;
        });
}


registerui::~registerui() {
    delete ui;
}

void registerui::on_pushButton_ok_clicked()
{
    if (this->ui->lineEdit_username_at_registerui->text().isEmpty())
    {
        showTip(tr("用户名不能为空！"), false);
        return;
    }
    this->mistake_mgr->delete_mistake(ErrorCodes::UserNameIsNull);
    if (this->ui->lineEdit_passwd_at_registerui->text().isEmpty())
    {
        showTip(tr("密码不能为空！"), false);
        return;
	}
    this->mistake_mgr->delete_mistake(ErrorCodes::UserPasswdError);
    if (this->ui->lineEdit_verify_passwd_at_registerui->text().isEmpty())
    {
        showTip(tr("请再次输入密码！"), false);
        return;
    }
    this->mistake_mgr->delete_mistake(ErrorCodes::UserPasswdError);
    if (this->ui->lineEdit_passwd_at_registerui->text() != this->ui->lineEdit_verify_passwd_at_registerui->text())
    {
        showTip(tr("两次输入的密码不一致！"), false);
        return;
    }
    this->mistake_mgr->delete_mistake(ErrorCodes::UserPasswdError);
    if(this->ui->lineEdit_verify_passwd_at_registerui->text().isEmpty())
        {
        showTip(tr("验证密码不能为空！"), false);
        return;
	}
    this->mistake_mgr->delete_mistake(ErrorCodes::UserPasswdError);

	// 发送注册请求
	QJsonObject jsonObject;
    jsonObject["user"] = this->ui->lineEdit_username_at_registerui->text();
    jsonObject["passwd"] =this->ui->lineEdit_passwd_at_registerui->text();
    jsonObject["email"] = this->ui->lineEdit_the_email_at_registerui->text();
    jsonObject["confirm"] = this->ui->lineEdit_verify_passwd_at_registerui->text();
    jsonObject["varify_code"] = this->ui->lineEdit_verifiycation_code_at_registerui->text();
    emit HttpMgr::getInstance()->slots_sendRequest(QUrl(gate_url_prefix+"/user_register"), jsonObject
		, ReqId::ID_REG_USER, Modules::MODULE_REGISTER);    
}
