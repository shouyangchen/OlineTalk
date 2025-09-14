#ifndef GIF_PLAYER_H
#define GIF_PLAYER_H
#include <QMovie>
#include <QObject>
#include <QMap>
#include <QTimer>
#include "gif_play_view.h"
#include "global.h"
#include "singleton.h"

class gif_player: public QObject,public singleton<gif_player>
{
	Q_OBJECT
private:
	friend class singleton<gif_player>;
	gif_player();
	QMap<the_gif_order, std::shared_ptr<QMovie>>movie_map;
	void  init_the_movie_map();
	gif_play_view* view;
public:
	gif_player(gif_player&) = delete;
	gif_player(gif_player&&) = delete;
	gif_player& operator=(gif_player&) = delete;
    ~gif_player()override;
signals:
    void sig_stop_display();
    void sig_update_gif(the_gif_order& order);

public slots:
	void play_the_gif(const the_gif_order& order,QWidget*ui);
	 
};


#endif

