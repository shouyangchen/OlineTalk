//
// Created by chenshouyang on 25-6-24.
//

#include "HttpMgr.h"
#include <utility>
HttpMgr::~HttpMgr() = default;
HttpMgr::HttpMgr() :QObject(nullptr) ,singleton<HttpMgr>(){
	connect(this,&HttpMgr::http_finish,this,&HttpMgr::do_when_http_finish);
    connect(this, &HttpMgr::sendRequest, this, &HttpMgr::slots_sendRequest);
}

void HttpMgr::slots_sendRequest(const QUrl &url, const QJsonObject &jsonObject, ReqId reqId, Modules module) {
    //发送请求的逻辑
    QByteArray jsonData = QJsonDocument(jsonObject).toJson();//先将json对象转换为json字符串
    QNetworkRequest request(url);//创建请求
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(jsonData.size()));//设置请求头
    auto self=this->shared_from_this(); // 获取 shared_ptr 的实例,增加引用计数防止析构
    //此处的self是为了防止在请求完成之前HttpMgr被析构导致的悬空指针问题并且在多线程调用的能够分清楚调用时this的数据状态
    QNetworkReply *reply = networkManager.post(request, jsonData);//发送post请求
    this->connect(reply,&QNetworkReply::finished,[reply,self,reqId,module] {
        //错误处理
            if (reply->error() != QNetworkReply::NoError) {
                qDebug()<<reply->errorString();
                emit self->http_finish(reqId,"",ErrorCodes::ERR_NETWORK,module);
                reply->deleteLater();
                return;
            }
        //无错误则读回请求
        QString res=reply->readAll();
        //发送信号通知完成
        emit self->http_finish(reqId,res,ErrorCodes::SUCCESS,module);
    });
}


void HttpMgr::do_when_http_finish(ReqId req_id, QString res, ErrorCodes err, Modules mod) {

    switch (mod) {//通过模块判断向指定模块发送信号
        case Modules::MODULE_REGISTER:
            emit reg_mod_finish(req_id,std::move(res),ErrorCodes::SUCCESS);
            break;
		case Modules::MODULE_LOGIN:
			emit login_mod_finish(req_id, std::move(res), ErrorCodes::SUCCESS);
            qDebug() << "Received login_mod_finish signal, err:" << err;
            break;
		case Modules::MODULE_FORGET_PASSWD:
			emit forget_passwd_mod_finsh(req_id, std::move(res), ErrorCodes::SUCCESS);
			break;
    }
}
