#ifndef CHAT_UI_H
#define CHAT_UI_H

#include <QWidget>
#include <QMouseEvent>
#include <QApplication>
#include <QRandomGenerator>
namespace Ui {
class chat_ui;
}


enum  CHAT_UI_MODE
{
	SerachMode,
    ChatMode,
    ConnectMode,
};


class chat_ui : public QWidget  
{
    Q_OBJECT
private:
	friend class mainwindow;
    void connect_sig();
    void set_qss();
    QPoint last_point__pos;
    bool moving = {};
    bool is_loading;
    QHash<QString, int>*the_stack_widget_index;
public:
    explicit chat_ui(QWidget *parent = nullptr);
    void add_user_list();
    ~chat_ui();
public  slots:
    void when_touch_close();
    void slot_loading_the_user_list();

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void closeEvent(QCloseEvent* event) override;// 在 chat_ui.h 的 protected 部分添加：
    bool eventFilter(QObject* watched, QEvent* event) override;
private:
    Ui::chat_ui *ui;
    void show_serach(bool is_serach);
    CHAT_UI_MODE mode;//模式
    CHAT_UI_MODE state;//状态
    void init_the_hash_of_stack_widget();
    void init_add_user_item();
};

#endif // CHAT_UI_H
