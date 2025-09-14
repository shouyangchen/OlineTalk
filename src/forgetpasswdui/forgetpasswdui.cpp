#include "forgetpasswdui.h"

#include "HttpMgr.h"

forgetpasswdui::forgetpasswdui(QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::forgetpasswduiClass())
{
	ui->setupUi(this);
	this->mistake_mgr = new MistakeMgr(this);
	this->set_the_input_line();
	this->connect_signal();
	this->init_handlers();
}

forgetpasswdui::~forgetpasswdui()
{
	delete ui;
}

void forgetpasswdui::closeEvent(QCloseEvent* event)
{
	auto parent = this->parentWidget();
	if (parent->isHidden())
	{
		parent->parentWidget()->show();
		parent->show();
		QDialog::closeEvent(event);
	}
	else
		QDialog::closeEvent(event);
}

void forgetpasswdui::showTip(QString str, bool isok)
{
	if (!isok)
		QMessageBox::warning(this, "错误！", str);
	else
		QMessageBox::information(this, "提示！", str);
}


void forgetpasswdui::connect_signal()
{
	connect(this->ui->lineEdit_id_binded_email, &InputLine::editingFinished, this, [this]()// 检查邮箱格式
	{
		auto email = this->ui->lineEdit_id_binded_email->text();
		QRegularExpression regex(R"([A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,})");
		auto match = regex.match(email).hasMatch();
		if (match)
			this->mistake_mgr->delete_mistake(ErrorCodes::EmailStlyeNotRight);
		else
			return;
		});
	connect(this->ui->lineEdit_passwd_forget, &InputLine::editingFinished, this, [this]()// 检查密码是否为空
	{
		auto passwd = this->ui->lineEdit_passwd_forget->text();
		if (passwd.isEmpty())
			return;
		else
			this->mistake_mgr->delete_mistake(ErrorCodes::UserPasswdIsNull);
		
	});
	connect(this->ui->lineEdit_varifiy_passwd_forget, &InputLine::editingFinished, this, [this]()// 检查验证密码是否为空
	{
		auto passwd = this->ui->lineEdit_varifiy_passwd_forget->text();
		if (passwd.isEmpty())
			return;
		if (passwd != this->ui->lineEdit_passwd_forget->text())
			return;
		else
			this->mistake_mgr->delete_mistake(ErrorCodes::UserPasswdError);
		});
	connect(this->ui->lineEdit_user_account_forget, &InputLine::editingFinished, this, [this]()// 检查用户名是否为空
	{
		auto user_account = this->ui->lineEdit_user_account_forget->text();
		if (user_account.isEmpty())
			return;
		else
			this->mistake_mgr->delete_mistake(ErrorCodes::UserNameIsNull);
		});
	connect(this->ui->pushButton_get_varifiycode_forget, &QPushButton::clicked, this, &forgetpasswdui::get_varifiy_code_forget_passwd);// 获取忘记密码的验证码
	connect(HttpMgr::getInstance().get(), &HttpMgr::forget_passwd_mod_finsh, this, &forgetpasswdui::get_the_http_reslut);// 连接HTTP请求完成的信号
	connect(this->ui->pushButton_submit_the_change_passwod_forget, &QPushButton::clicked, this, &forgetpasswdui::submit_the_change_passwd_request);// 提交修改密码请求
}


void forgetpasswdui::set_the_input_line()
{
	this->ui->lineEdit_passwd_forget->setEchoMode(QLineEdit::Password);
	this->ui->lineEdit_varifiy_passwd_forget->setEchoMode(QLineEdit::Password);
}


void forgetpasswdui::get_varifiy_code_forget_passwd()
{
	QJsonObject jsonObject;
	jsonObject["email"] = this->ui->lineEdit_id_binded_email->text();
	jsonObject["uid"] = this->ui->lineEdit_user_account_forget->text();
	HttpMgr::getInstance()->sendRequest(QUrl(gate_url_prefix + "/get_varify_code_forget_passwd"), jsonObject
		, ReqId::ID_GET_VARIFY_CODE_FORGET_PASSWD, Modules::MODULE_FORGET_PASSWD);

}

void forgetpasswdui::get_the_http_reslut(const ReqId& req, const QString& res, const ErrorCodes& err)
{
	switch (req)
	{
		case ReqId::ID_GET_VARIFY_CODE_FORGET_PASSWD:
		{
			QJsonObject json_object = QJsonDocument::fromJson(res.toUtf8()).object();
				if (err != ErrorCodes::SUCCESS)
			{
				this->showTip("获取验证码失败，请检查网络连接或稍后再试！", false);
				return;
				}
			this->handlers[req](json_object);
			break;
		}
		case ReqId::ID_SUBMIT_FORGET_PASSWD:
		{
			QJsonObject json_object = QJsonDocument::fromJson(res.toUtf8()).object();
			if (err != ErrorCodes::SUCCESS)
			{
				this->showTip("修改密码失败，请检查网络连接或稍后再试！", false);
				return;
			}
			this->handlers[req](json_object);
			break;
		}
		default:
			break;
	}
}

void forgetpasswdui::init_handlers()
{
	this->handlers.insert(ReqId::ID_GET_VARIFY_CODE_FORGET_PASSWD, [this](QJsonObject& json_object)// 获取忘记密码的验证码
		{
			if (json_object.isEmpty())
			{
				this->showTip("JSON解析失败", false);
				return;
			}
			else
			{
				auto email = json_object["email"].toString();
				auto err = static_cast<ErrorCodes>(json_object["err"].toInt());
				if (err!=ErrorCodes::SUCCESS)
				{
					this->showTip("获取验证码失败，请检查网络连接或稍后再试！", false);
					return;
				}
				this->showTip(QString("验证码已经发送至你的邮箱%1,请您注意查收！").arg(email), true);
			}

		});
	this->handlers.insert(ReqId::ID_SUBMIT_FORGET_PASSWD, [this](QJsonObject& json_object)// 提交修改密码请求
		{
			if (json_object.isEmpty())
			{
				this->showTip("JSON解析失败", false);
				return;
			}
			else
			{
				auto err = static_cast<ErrorCodes>(json_object["err"].toInt());
				if(err==ErrorCodes::UserNotExist)
				{
					this->showTip(QString("该账号未绑定至改邮箱或者账户号不存在!"), false);
					return;
				}
				if (err==VarifyError)
				{
					this->showTip(QString("验证码错误，请重新获取验证码!"), false);
					return;
				}
				if (err != ErrorCodes::SUCCESS)
				{
					this->showTip("修改密码失败，请检查网络连接或稍后再试！", false);
					return;
				}
				this->showTip("密码修改成功，请牢记您的新密码！", true);
			}
		});
}

void forgetpasswdui::submit_the_change_passwd_request()
{
	auto email = this->ui->lineEdit_id_binded_email->text();
	auto user_account = this->ui->lineEdit_user_account_forget->text();
	auto varify_code = this->ui->lineEdit_varifiy_code_forget->text();
	auto passwd = this->ui->lineEdit_passwd_forget->text();
	if (email.isEmpty() || user_account.isEmpty() || varify_code.isEmpty())
	{
		this->showTip("请填写完整信息！", false);
		return;
	}
	QJsonObject jsonObject;
	jsonObject["email"] = email;
	jsonObject["uid"] = user_account;
	jsonObject["varify_code"] = varify_code;
	jsonObject["new_passwd"] = this->ui->lineEdit_varifiy_passwd_forget->text();
	HttpMgr::getInstance()->sendRequest(QUrl(gate_url_prefix + "/submit_forget_passwd"), jsonObject
		, ReqId::ID_SUBMIT_FORGET_PASSWD, Modules::MODULE_FORGET_PASSWD);
}
