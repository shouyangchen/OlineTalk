#ifndef TEXT_BUBBLE_H
#define TEXT_BUBBLE_H
#include <QLabel>
#include "BubbleFrame.h"
#include <QHBoxLayout>

class TextBubble:public BubbleFrame
{
    Q_OBJECT
private:
    QLabel* text_label_m;
public:
    TextBubble(ChatRole role, const QString& text, QWidget* parent = nullptr);
private:
    void set_plain_text(const QString& text);
};

#endif

