//
// Created by chenshouyang on 25-6-23.
//

// You may need to build the project (run Qt uic code generator) to get "ui_loginui.h" resolved

#include "loginui.h"
#include "ui_loginui.h"
#include "global.h"
#include "HttpMgr.h"
#include <QMessageBox>
#include "ClickedLabel.h"
#include "the_user_icon_mgr.h"
#include "forgetpasswdui.h"
#include <memory>
#include <QTimer>

#include "gif_player.h"
#include "mainwindow.h"
#include "TcpMgr.h"
#include "user_info_mgr.h"


loginui::loginui(QWidget *parent) :
    QDialog(parent), ui(new Ui::loginui) {
    ui->setupUi(this);

    this->set_the_passwd_input_line();
    this->setMaximumSize(QSize(350, 550));
    this->setMinimumSize(QSize(350, 550));
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowMaximizeButtonHint);
    this->init_handlers();
    this->connect_signals();
}

void loginui::set_the_passwd_input_line() {
    this->ui->lineEdit_inputpasswd->setEchoMode(QLineEdit::Password);
}

void loginui::connect_signals() {
    // Connect signals and slots here if needed
    connect(this->ui->pushButton_register,&QPushButton::clicked,this,[this] {
        auto *register_ui = new registerui(this);
        this->hide(); // Hide the login UI
        this->parentWidget()->hide();// Hide the parent widget if needed
        register_ui->show();
    });
    connect(this->ui->pushButton_login, &QPushButton::clicked, this, &loginui::on_pushButton_login_clicked);
	connect(HttpMgr::getInstance().get(), &HttpMgr::login_mod_finish, this, &loginui::do_when_login_mod_finish);
	connect(this, &loginui::have_the_user_icon, this->ui->frame, &user_icon_frame::draw_user_icon);
    
    // 提供两种方案供选择
    connect(this->ui->label_fogetpasswd, &ClickedLabel::Clicked, this, [this]
    {
        if (this->forget_passwd_ui == nullptr)
        {
            this->forget_passwd_ui = new forgetpasswdui(this);
        }
  
        this->forget_passwd_ui->setWindowModality(Qt::ApplicationModal);
        int result = this->forget_passwd_ui->exec();  // 使用exec()
        if (result == QDialog::Accepted) {
            qDebug() << "Password reset completed";
        }

    });
    
    connect(this->ui->lineEdit_username,&InputLine::editingFinished,this,&loginui::do_when_the_account_input_done);
    connect(this,&loginui::have_the_user_icon,this->ui->frame,&user_icon_frame::draw_user_icon);
    connect(TcpMgr::getInstance().get(), &TcpMgr::sig_connect_server, TcpMgr::getInstance().get(), &TcpMgr::connect_to_server);
}

void loginui::on_pushButton_login_clicked() {//新增加利用uid或者邮箱登录
    auto username = this->ui->lineEdit_username->text();
    auto passwd = this->ui->lineEdit_inputpasswd->text();
    if (username.isEmpty()) {
        this->set_the_wrong_tip(tr("用户名不能为空!"));
        QApplication::beep();
        return;
    }
    if (passwd.isEmpty()) {
        this->set_the_wrong_tip(tr("密码不能为空!"));
        QApplication::beep();
        return;
    }
    QJsonObject jsonObject;
    // Send login request
    if (adjust_is_email(username))//如果为邮箱登录
    {
        jsonObject["email"] = username;
        jsonObject["is_email"] = 1;
    }
    else {
        jsonObject["uid"] = username;
        jsonObject["is_email"] = 0;
    }
    jsonObject["passwd"] = passwd;
	emit HttpMgr::getInstance()->sendRequest(QUrl(gate_url_prefix + "/user_login"), jsonObject,ReqId::ID_USER_LOGIN,Modules::MODULE_LOGIN);
    gif_player::getInstance()->play_the_gif(the_gif_order::THE_LOADING_GIF, this->parentWidget());
}

void loginui::init_handlers()
{
    this->handlers.insert(ReqId::ID_USER_LOGIN, [this](const QJsonObject &obj) {
        auto error = obj["error"].toInt();
        if (error != ErrorCodes::SUCCESS) {
            if(error==ErrorCodes::UserNotExist) {
                   this->set_the_wrong_tip("用户不存在！");
            } else if (error == ErrorCodes::UserPasswdError) {
                this->set_the_wrong_tip("错误!密码错误，请重试！");
            } else {
                this->set_the_wrong_tip("错误! 登录失败，请稍后再试！");
			}
            emit gif_player::getInstance()->sig_stop_display();
            return;
        }

        auto uid = static_cast<std::uint64_t>(obj["uid"].toDouble());
        qDebug() << "Received UID from JSON:" << uid ;
        auto token = obj["token"].toString();
        auto host = obj["host"].toString();

    	auto user_info = user_info_mgr::getInstance(QPixmap{}, QString{this->ui->lineEdit_username->text()}, QString{}, uid);
        // 处理端口 - 可能是字符串也可能是数字
        user_info->set_user_uid(uid);
        auto portValue = obj["port"];
        int port = 0;
        if (portValue.isString()) {
            bool ok;
            port = portValue.toString().toInt(&ok);
            if (!ok) {
                qDebug() << "Failed to convert port string to int:" << portValue.toString();
                this->set_the_wrong_tip("服务器端口格式错误！");
                emit gif_player::getInstance()->sig_stop_display();
                return;
            }
        } else if (portValue.isDouble()) {
            port = portValue.toInt();
        } else {
            qDebug() << "Invalid port type in JSON response";
            this->set_the_wrong_tip("服务器响应格式错误！");
            emit gif_player::getInstance()->sig_stop_display();
            return;
        }
        
        qDebug() << QJsonDocument(obj).toJson();
        qDebug() << "Port value from JSON:" << port << "Type:" << (portValue.isString() ? "String" : "Number");
        server_info server{ host,static_cast<quint16>(port) };
        qDebug() << "User logged in successfully with ID:" << uid;
        qDebug() << "Server info - Host:" << server.host << "Port:" << server.port;
        
        // Check if we're already connected before trying to connect
        auto tcpMgr = TcpMgr::getInstance();
        if (tcpMgr->isConnected()) {
            qDebug() << "Already connected to TCP server, disconnecting first...";
            tcpMgr->disconnect_from_server();
        }
        TcpMgr::getInstance()->sig_connect_server(server);
        QJsonDocument doc(obj);

        // Add a small delay before sending data to ensure connection is established
        QTimer::singleShot(1000, [this, doc]() {
            auto tcpMgr = TcpMgr::getInstance();
            if (tcpMgr->isConnected()) {
                tcpMgr->send_data(Message_id::LOGIN_MESSAGE, doc.toJson());
                qDebug() << "Login data sent to TCP server";
            } else {
                qDebug() << "Failed to establish TCP connection for sending login data";
            }
        });
        
        qDebug() << server.host;
		});

    this->handlers.insert(ReqId::ID_GET_USER_ICON, [this](const QJsonObject&obj)
        {
            if (!(obj["error"].toInt()==ErrorCodes::SUCCESS))
            {
                qDebug() << "can't get user icon";
                return;
            }
            else
            {
                auto user_icon_data = obj["user_icon"].toVariant().toByteArray();
				QByteArray byteArray = QByteArray::fromBase64(user_icon_data);//对用户头像进行base64解码
                QPixmap user_icon;
                if (!user_icon.loadFromData(byteArray))
                {

                    qDebug() << "Failed to load user icon from data";
                    return;
                }
                auto account = this->ui->lineEdit_username->text();
                auto icon_mgr = the_user_icon_mgr::getInstance();
                if (adjust_is_email(account))
                {
					user_icon = icon_mgr->get_user_icon(QString{}, account);//如果是邮箱登录则传入邮箱
                }
                else
				{
                    user_icon = icon_mgr->get_user_icon(account, QString{});//如果是账号登录则传入账号
				}
                emit this->have_the_user_icon(user_icon);
                user_info_mgr::getInstance(user_icon, account, QString{}, std::int64_t{});
				qDebug() << "user icon get success";
            }
        });
}


void loginui::do_when_login_mod_finish(ReqId req_id, QString res, ErrorCodes err) {
	if (err != ErrorCodes::SUCCESS) {
         this->set_the_wrong_tip("网络错误！更换网络环境之后再次重试！");
		return;
	}
	//解析JSON响应，res是服务器返回的JSON字符串需要转化为QByteArray
	QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
	if (jsonDoc.isNull()) {
        this->set_the_wrong_tip("无法连接服务器！");
		return;
	}
	if (!jsonDoc.isObject()) {
        this->set_the_wrong_tip("Json解析错误！");
		return;
	}
	QJsonObject jsonObj = jsonDoc.object();
	handlers[req_id](jsonObj);
}


void loginui::set_the_wrong_tip(const QString& str)
{
    this->ui->login_ui_tip->setStyleSheet(
        "QLabel {"
        "    color: #d32f2f; /* 错误提示红色 */"
        "    background: #ffebee; /* 浅红色背景 */"
        "    border-radius: 8px;"
        "    padding: 4px 8px;"
        "}"
        );
    this->ui->login_ui_tip->setText(str);
}





loginui::~loginui() {
    delete ui;
}


void loginui::do_get_the_user_icon()
{
    //该方法主要是当用户输入完之后被设置了默认头像之后就开是从服务器获取头像然后更新本地数据库头像
    auto user_id = this->ui->lineEdit_username->text();
	if (adjust_is_email(user_id))//如果为邮箱登录
    {
        QJsonObject user_icon_json;
        user_icon_json["email"] = user_id;
        user_icon_json["uid"] = "NULL";
        user_icon_json["is_email"] = 1;
        emit HttpMgr::getInstance()->sendRequest(QUrl(gate_url_prefix + "/get_user_icon"), user_icon_json, ReqId::ID_GET_USER_ICON, Modules::MODULE_LOGIN);//发送获取头像请求
    }
	else//如果为账号登录
    {
	    QJsonObject user_icon_json;
		user_icon_json["email"] = "NULL";
	    user_icon_json["uid"] = user_id;
	    user_icon_json["is_email"] = 0;
	    emit HttpMgr::getInstance()->sendRequest(QUrl(gate_url_prefix + "/get_user_icon"), user_icon_json, ReqId::ID_GET_USER_ICON, Modules::MODULE_LOGIN);//发送获取头像请求
    }
}



void loginui::do_when_the_account_input_done()
{
	auto account = this->ui->lineEdit_username->text();
    if (account.isEmpty())
        return;
	auto icon_mgr = the_user_icon_mgr::getInstance();
	if (adjust_is_email(account))
	{
		this->user_icon = icon_mgr->get_user_icon(QString{}, account);// 如果是邮箱登录则传入邮箱
		emit this->have_the_user_icon(this->user_icon);
	}
    else
    {
	    this->user_icon = icon_mgr->get_user_icon(account, QString{});// 如果是账号登录则传入账号
	    emit this->have_the_user_icon(this->user_icon);
    }
    this->do_get_the_user_icon();
}
