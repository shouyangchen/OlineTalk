#include "TextBubble.h"
#include <QDebug>

TextBubble::TextBubble(ChatRole role, const QString& text, QWidget* parent)
    : BubbleFrame(role, parent)
{
    this->text_label_m = new QLabel();
    this->set_plain_text(text);
    this->set_widget(this->text_label_m);
    QFont font("Microsoft YaHei");
    font.setPointSize(12);
    this->text_label_m->setFont(font);
    this->text_label_m->setStyleSheet("QLabel{border:none;background:transparent;}");
    this->text_label_m->setWordWrap(true);
    this->text_label_m->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    int max_bubble_width = 600;
    this->text_label_m->setMaximumWidth(max_bubble_width);
    this->text_label_m->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    this->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
}

void TextBubble::set_plain_text(const QString& text)
{
    qDebug() << "TextBubble::set_plain_text, text:" << text;
    this->text_label_m->setText(text);
}