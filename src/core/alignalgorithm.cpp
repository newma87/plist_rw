#include <QtGui>
#include "core/alignalgorithm.h"
#include "core/frame.h"
#include "core/framecollector.h"
#include "util/util.h"

QRect NormalAlignAlgorithm::findPropertyPlace(const FrameCollector &collector, const QPoint &mousePos, const QSize &imageSize)
{
    return NormalAlignAlgorithm::findPropertyPlace(collector, QRect(mousePos.x() - imageSize.width() / 2, mousePos.y() - imageSize.height() / 2, imageSize.width(), imageSize.height()));
}

QRect NormalAlignAlgorithm::adjustRectInRange(const QRect& target, const QRect& rangeRect)
{
    if ((rangeRect.width() - target.width() < 0) || (rangeRect.height() - target.height() < 0))
    {
        //todo log, this situation is not allowed
        return QRect();
    }

    int transX = 0;
    int transY = 0;

    if (target.left() < rangeRect.left())
    {
        transX = rangeRect.left() - target.left();
    }

    if (target.right() > rangeRect.right())
    {
        transX = rangeRect.right() - target.right();
    }

    if (target.top() < rangeRect.top())
    {
        transY = rangeRect.top() - target.top();
    }

    if (target.bottom() > rangeRect.bottom())
    {
        transY = rangeRect.bottom() - target.bottom();
    }

    return target.translated(transX, transY);
}

int NormalAlignAlgorithm::nearAlign(int l1, int l2, int base, int* translate_dist)
{
    int d1 = l1 - base;
    int d2 = l2 - base;

    int lenth = MIN(d1 * d1, d2 * d2);

    if (lenth > (algin_lenth * algin_lenth))
    {
        return 0;
    }

    if (lenth == d1 * d1)
    {
        if (translate_dist)
        {
            *translate_dist = -d1;
        }
        return 1;
    }
    else
    {
        if (translate_dist)
        {
            *translate_dist = -d2;
        }
        return 2;
    }

    return 0;
}

QRect NormalAlignAlgorithm::findPropertyPlace(const FrameCollector &collector, const QRect &imageRect)
{
    QSize size = collector.getSize();
    QRect rect = QRect(QPoint(0, 0), size);

    QRect adjRect = NormalAlignAlgorithm::adjustRectInRange(imageRect, rect);
    if (!adjRect.isValid())
    {
        //todo warning
        return adjRect;
    }

    if (collector.count() <= 0)
    {
        return adjRect;
    }

    QList<Frame*> conflictList;
    if (!collector.findFramesInRect(adjRect, &conflictList))
    {
        return adjRect;
    }

    QRect conflictRect = conflictList.front()->rect;

    int transX = 0;
    int transY = 0;

    QPoint distance = adjRect.center() - conflictRect.center();

    if (distance.x() * distance.x() >= distance.y() * distance.y())
    {
        if (distance.x() < 0)
        {
            transX = conflictRect.left() - adjRect.right() - 1;
        }
        else
        {
            transX = conflictRect.right() + 1 - adjRect.left();
        }

        // vertical align
        // attention: rect.left() and rect.righ() is still in rect. Otherwise rect.right() - rect.left() = rect().width - 1. The same to top and bottom.
        if (!NormalAlignAlgorithm::nearAlign(adjRect.top(), adjRect.bottom() + 1, conflictRect.top(), &transY))
        {
            NormalAlignAlgorithm::nearAlign(adjRect.top(), adjRect.bottom() + 1, conflictRect.bottom() + 1, &transY);
        }
    }
    else
    {
        if (distance.y() < 0)
        {
            transY = conflictRect.top() - adjRect.bottom() - 1;
        }
        else
        {
            transY = conflictRect.bottom() + 1 - adjRect.top();
        }

        //horizontal align
        // attention: rect.left() and rect.righ() is still in rect. Otherwise rect.right() - rect.left() = rect().width - 1. The same to top and bottom.
        if (!NormalAlignAlgorithm::nearAlign(adjRect.left(), adjRect.right() + 1, conflictRect.left(), &transX))
        {
            NormalAlignAlgorithm::nearAlign(adjRect.left(), adjRect.right() + 1, conflictRect.right() + 1, &transX);
        }
    }

    if (!collector.findFramesInRect(adjRect.translated(transX, transY)))
    {
        return adjRect.translated(transX, transY);
    }

    if (transX != 0 && transY != 0)
    {
        if (!collector.findFramesInRect(adjRect.translated(transX, 0)))
        {
            return adjRect.translated(transX, 0);
        }

        if (!collector.findFramesInRect(adjRect.translated(0, transY)))
        {
            return adjRect.translated(0, transY);
        }
    }

    return QRect();
}
