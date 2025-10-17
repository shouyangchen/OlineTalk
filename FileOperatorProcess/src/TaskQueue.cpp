#include "FileOperatorProcess/include/TaskQueue.h"

TaskQueue::TaskQueue(QObject* parent)
    : QObject(parent), m_queue()
{
}

void TaskQueue::push_back(const Request& value)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push_back(value);
	emit queueChanged(m_queue.front());//当由队列添加新任务时，发出信号通知就可以避免轮询的方式获取任务
}

bool TaskQueue::empty() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.isEmpty();
}

Request TaskQueue::front() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.head();
}

Request TaskQueue::dequeue()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.dequeue();
}
