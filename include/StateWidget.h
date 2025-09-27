#ifndef STATEWIDGET_H
#define STATEWIDGET_H
#include <QWidget>
#include <QLabel>
#include <QEvent>
#include "chat_ui.h"
#include "ClickedLabel.h"
#include "DisplayMessageNumsLabel.h"
#include <QCache>
#include <QHBoxLayout>
#include <QDragEnterEvent>
#include <QMimeData>

class StateWidget : public QWidget {
	Q_OBJECT
	friend class chat_ui;
	
private:
	std::atomic<unsigned char> the_message_count_m;
	DisplayMessageNumsLabel* display_the_message_icon_label_m;
public:
	StateWidget(QWidget* parent = nullptr);
	~StateWidget();
	void increment_message_count();
	void clear_message_count();
	void set_message_count(unsigned char count);
	void setPixmap(const QPixmap& pixmap);
	unsigned char get_message_count() const;
	signals:
		void the_state_widget_was_touched();
protected:
	bool eventFilter(QObject* watched, QEvent* event) override;
};

#endif