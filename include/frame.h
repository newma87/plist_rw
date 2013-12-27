/*
 *  frame.h
 *  plist_rw version 1.0
 *  author by newma
 *  email to newma@live.cn
 */

#ifndef FRAME_H
#define FRAME_H

#include <QPixmap>
#include <QString>
#include <QRect>

struct Frame
{
    QPixmap image;
    QString name;
    QRect rect;
};

inline bool isRectCross(const QRect& rect1, const QRect& rect2)
{
    int x1 = 0;
    int x2 = 0;
    int y1 = 0;
    int y2 = 0;

    x1 = rect1.left() > rect2.left() ? rect1.left() : rect2.left();
    y1 = rect1.top() > rect2.top() ? rect1.top() : rect2.top();
    x2 = rect1.right() < rect2.right() ? rect1.right() : rect2.right();
    y2 = rect1.bottom() < rect2.bottom() ? rect1.bottom() : rect2.bottom();

    if (x1 < x2 && y1 < y2)
    {
        return true;
    }
    return false;
}

inline bool isPosInRect(const QPoint& pos, const QRect& rect)
{
    int x = pos.x();
    int y = pos.y();

    if ((x > rect.left() && x < rect.right()) && (y > rect.top() && y < rect.bottom()))
    {
        return true;
    }
    return false;
}

#endif // FRAME_H
