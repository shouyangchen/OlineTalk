#ifndef GIF_PLAY_VIEW_H
#define GIF_PLAY_VIEW_H

#include <QDialog>
#include <QPointer>
	#include "ui_gif_play_view.h"
#include <QLabel>
namespace Ui {
class gif_play_view;
}

class gif_play_view : public QDialog
{
    Q_OBJECT
private:
public:
    explicit gif_play_view(QWidget *parent = nullptr);
    ~gif_play_view();
    void set_gif(std::shared_ptr<QMovie>gif);
private:
    Ui::gif_play_view *ui;
};

#endif // GIF_PLAY_VIEW_H
