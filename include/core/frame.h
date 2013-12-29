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
    bool bValid;

    Frame() : bValid(false)
    {}

    Frame(const Frame&  frame)
    {
        if (this != &frame)
        {
            image = frame.image;
            name = frame.name;
            rect = frame.rect;
            bValid = frame.bValid;
        }
    }
};

#endif // FRAME_H
