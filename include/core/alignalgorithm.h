/*
 *  alignalgorithm.h
 *  plist_rw version 1.0
 *  author by newma
 *  email to newma@live.cn
 */

#ifndef ALIGNALGORITHM_H
#define ALIGNALGORITHM_H

#include <QRect>

class FrameCollector;
class NormalAlignAlgorithm
{
public:
    enum {
        algin_lenth = 5   // align base lenth (pixel)
    };

    static QRect findPropertyPlace(const FrameCollector& collector, const QPoint& mousePos, const QSize& imageSize);
    static QRect findPropertyPlace(const FrameCollector& collector, const QRect& imageRect);

protected:
    static QRect adjustRectInRange(const QRect& target, const QRect& ragneRect);
    static int nearAlign(int l1, int l2, int base, int* translate_dist);
};

#endif // ALIGNALGORITHM_H
