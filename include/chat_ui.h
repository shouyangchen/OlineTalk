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
    void connect_sig();
    void set_qss();
    QPoint last_point__pos;
    bool moving = {};
    bool is_loading;
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
private:
    Ui::chat_ui *ui;
    void show_serach(bool is_serach);
    CHAT_UI_MODE mode;//模式
    CHAT_UI_MODE state;//状态
};

#endif // CHAT_UI_H
