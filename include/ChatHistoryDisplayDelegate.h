#ifndef CHAT_HISTORY_DISPLAY_DELEGATE_H
#define CHAT_HISTORY_DISPLAY_DELEGATE_H
#include <QStyledItemDelegate>
#include <QPainter>
#include "the_user_icon_mgr.h"
#include <QPixmap>


class ChatHistoryDisplayDelegate :public QStyledItemDelegate
{
	Q_OBJECT
private:
	QPixmap my_icon;
	QSize calculate_text_widget_size(QString const& text)const;
	QSize calculate_image_widget_size(QByteArray const& image_data)const;
	QSize calculate_video_widget_size(QByteArray const& video_data)const;
	QSize calculate_file_widget_size(QString const& file_name)const;
	QSize calculate_audio_widget_size(QByteArray const& audio_data)const;
public:
	ChatHistoryDisplayDelegate(QObject* parent = nullptr);
	~ChatHistoryDisplayDelegate()override = default;
	signals:
		void replace_the_user_icon(const QPixmap& icon);//请求更换用户头像
		void replace_the_my_icon(const QPixmap& icon);
protected:
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QSize sizeHint(const QStyleOptionViewItem& option,const QModelIndex& index) const override;

public slots:
	void slot_replace_the_user_icon(const QPixmap& icon);//更换用户头像
	void slot_replace_the_my_icon(const QPixmap& icon);

};
#endif

