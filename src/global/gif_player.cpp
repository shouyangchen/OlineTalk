#include "gif_player.h"

#include <ui_gif_play_view.h>
#include <QPointer>

#include "gif_play_view.h"

gif_player::gif_player()
{
    this->init_the_movie_map();
}


void gif_player::init_the_movie_map()
{
	movie_map.insert(the_gif_order::THE_LOADING_GIF, std::make_shared<QMovie>(":res/loading.gif","GIF"));
	movie_map.insert(the_gif_order::THE_LOADING_DONE, std::make_shared<QMovie>());
}


void gif_player::play_the_gif(const the_gif_order& order, QWidget* ui)
{
    gif_play_view* the_display_view = new gif_play_view(ui);

    auto& gif = this->movie_map[order];
    the_display_view->set_gif(gif);
    gif->start();
    
    // 使用 QPointer 来安全管理对象生命周期
    QPointer<gif_play_view> safe_display_view(the_display_view);
    
    connect(this, &gif_player::sig_stop_display, [safe_display_view]()
    {
        if (safe_display_view) {
            safe_display_view->hide();         // 立即隐藏
            safe_display_view->deleteLater();  // 安全删除
        }
    });

    connect(this, &gif_player::sig_update_gif, [this, safe_display_view](const the_gif_order& order) {
        if (!safe_display_view) return; // 检查对象是否仍然有效
        
        auto& gif = this->movie_map[order];
        safe_display_view->set_gif(gif);
        gif->start();
        if (order == the_gif_order::THE_LOADING_DONE) {
            QTimer::singleShot(1000, [safe_display_view]()
                {
                    if (safe_display_view) {
                        safe_display_view->hide();
                        safe_display_view->deleteLater();
                    }
                });
        }
    });
    the_display_view->show();
}


gif_player::~gif_player()
{

}
