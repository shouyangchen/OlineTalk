#ifndef TASKQUEUE_H
#define TASKQUEUE_H
#include <mutex>
#include <QObject>
#include <QQueue>
#include "singleton.h"
#include <QDataStream>

struct Request
{
    enum Request_type:unsigned char
    {
        REQUEST_SEND_FILE,//上传文件
        REQUEST_DOWN_LOAD_FILE//下载文件
    };

    Request_type request_type;
    QString target_host;//对端地址
	quint16 target_port;//对端端口
	QString file_path;//文件路径
};

//重载QDataStream的输入输出运算符以支持Request对象的序列化和反序列化
//向MOC注册Request类型以支持信号槽传递

Q_DECLARE_METATYPE(Request)

inline QDataStream& operator<<(QDataStream& out, const Request& r)//序列化
{
    out << static_cast<quint8>(r.request_type);
    out << r.target_host;
	out << r.target_port;
	out << r.file_path;
    return out;
}

inline QDataStream& operator>>(QDataStream& in, Request& r)//反序列化
{
    quint8 t;
    in >> t;
    r.request_type = static_cast<Request::Request_type>(t);
    in >> r.target_host;
	in >> r.target_port;
	in >> r.file_path;
    return in;
}

//线程安全的任务队列且支持信号槽机制以实现事件驱动避免轮询

class TaskQueue : public QObject,public singleton<TaskQueue>
{
    Q_OBJECT
public:
    TaskQueue(const TaskQueue& other) = delete;
    TaskQueue& operator=(const TaskQueue& other) = delete;
	TaskQueue(TaskQueue&& other) = delete;
    void push_back(const Request& value);
    bool empty() const;
    Request front() const;
    Request dequeue();

    ~TaskQueue() override = default;

signals:
    void queueChanged(const Request&);

private:
	friend class singleton<TaskQueue>;
    TaskQueue(QObject* parent = nullptr);
    mutable std::mutex m_mutex; // protect queue
    QQueue<Request> m_queue;    // task queue
};

#endif
