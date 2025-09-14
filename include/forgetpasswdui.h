#pragma once

#include <QDialog>
#include "ui_forgetpasswdui.h"
#include "mistakekmgr.h"
#include "QMessageBox"
#include "global.h"
#include <QJsonObject>
#include <QJsonDocument>
QT_BEGIN_NAMESPACE
namespace Ui { class forgetpasswduiClass; };
QT_END_NAMESPACE

class forgetpasswdui : public QDialog
{
	Q_OBJECT
private:
	friend  class TimerButton;
	MistakeMgr* mistake_mgr; // 错误提示管理器
public:
	forgetpasswdui(QWidget *parent = nullptr);
	~forgetpasswdui();

public slots:
	void get_varifiy_code_forget_passwd();// 获取忘记密码的验证码
	void get_the_http_reslut(const ReqId& req, const QString& res,const ErrorCodes& err);
	void submit_the_change_passwd_request();// 提交修改密码请求
protected:
	void closeEvent(QCloseEvent* event) override;
private:
	Ui::forgetpasswduiClass *ui;
	void showTip(QString str, bool isok);
	void set_the_input_line();
	void connect_signal();
	QMap<ReqId, std::function<void(QJsonObject& jsonObject)>> handlers; // 用于存储处理函数的映射根据不同的请求id执行不同的逻辑
	void init_handlers();
};

