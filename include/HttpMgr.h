//
// Created by chenshouyang on 25-6-24.
//

#ifndef HTTPMGR_H
#define HTTPMGR_H
#include <QObject>
#include <QNetworkAccessManager>
#include "singleton.h"
#include "global.h"
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <memory>
#include <QString>
#include <QUrl>


class HttpMgr:public QObject,public singleton<HttpMgr>
    ,public std::enable_shared_from_this<HttpMgr>{
    Q_OBJECT
public:
    ~HttpMgr() override;
    //发送请求
public slots:
    void slots_sendRequest(const QUrl& url, const QJsonObject& jsonObject, ReqId reqId, Modules module);
    void do_when_http_finish(ReqId req_id,QString res,ErrorCodes err,Modules mod);
signals:
    void sendRequest(const QUrl& url, const QJsonObject& jsonObject, ReqId reqId, Modules module);
    void http_finish(ReqId,QString,ErrorCodes,Modules);//当请求完成时发送的信号
    void reg_mod_finish(ReqId,QString,ErrorCodes);//向注册模块发送完成http
	void login_mod_finish(ReqId, QString, ErrorCodes);//向登录模块发送完成http
	void forget_passwd_mod_finsh(ReqId, QString, ErrorCodes);//向忘记密码模块发送完成http
private:
    friend class singleton<HttpMgr>;//通过友元就可以让单例能够访问起构造函数创建HttpMgr了
    HttpMgr();
    QNetworkAccessManager networkManager;
};



#endif //HTTPMGR_H
