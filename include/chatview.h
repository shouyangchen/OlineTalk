#ifndef CHATVIEW_H
#define CHATVIEW_H

#include <QVBoxLayout>
#include <QWidget>
#include <QScrollArea>
class ChatView  : public QWidget
{
	Q_OBJECT
private:
	QVBoxLayout* pvl;
	QScrollArea* p_scroll_area;
	bool is_Appended;
	
	// 私有方法：创建消息组件
	QWidget* create_message_bubble(const QString& message, const QString& sender, const QString& timestamp = "");
	QWidget* create_system_message(const QString& message);

public:
	ChatView(QWidget *parent);
	
	// 基础方法
	void append_item(QWidget* item);
	void prepend_item(QWidget* item);
	void insert_ietm(QWidget* before, QWidget* item);
	
	// 便捷的消息添加方法
	void add_message(const QString& message, const QString& sender, const QString& timestamp = "");
	void add_system_message(const QString& message);
	
	~ChatView();

protected:
	bool eventFilter(QObject* watched, QEvent* event) override;

public slots:
	void on_VScroll_bar_moved(int min, int max);
};

#endif // CHATVIEW_H

