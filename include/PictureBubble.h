#ifndef PICTUREBUBBLE_H
#define PICTUREBUBBLE_H

#include "BubbleFrame.h"
#include "ClickedLabel.h"

class PictureBubble : public BubbleFrame
{
    Q_OBJECT
public:
    PictureBubble(const QPixmap& picture, ChatRole role, QWidget* parent = nullptr);
};

#endif // PICTUREBUBBLE_H