/*
 *  plist_rw version 1.0
 *  author by newma
 *  email to newma@live.cn
 */

#include <QtGui>
#include "frame.h"
#include "framecollector.h"

#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#define MIN(a, b) ((a) <= (b) ? (a) : (b))

inline unsigned int get2PowerValue(unsigned int uVal)
{
    int nCount = 0;
    while(uVal)
    {
        uVal = uVal >> 1;
        nCount++;
    }

    return (unsigned int)(1 << nCount);
}

FrameCollector::FrameCollector() : m_imageWidth(DEFAULT_IMAGE_SIZE), m_imageHeight(DEFAULT_IMAGE_SIZE)
{
    clear();
}

FrameCollector::~FrameCollector()
{
    clear();
}

bool FrameCollector::getImage(QImage** ppImage) const
{
    if (!ppImage)
    {
        return false;
    }
    *ppImage = new QImage(m_imageWidth, m_imageHeight, QImage::Format_ARGB32);
    (*ppImage)->fill(QColor(0, 0, 0, 0));

    QPainter paint(*ppImage);
    foreach(Frame* pF, m_frames)
    {
        paint.drawPixmap(pF->rect, pF->image);
    }

    return true;
}

QRect FrameCollector::getSmallestRangeRect() const
{
    int minX = m_frames[0]->rect.x();
    int maxX = minX;
    int minY = m_frames[0]->rect.y();
    int maxY = minY;
    foreach (Frame* pF, m_frames) {
        if (minX > pF->rect.left())
        {
            minX = pF->rect.left();
        }

        if (maxX < pF->rect.right())
        {
            maxX = pF->rect.right();
        }

        if (minY > pF->rect.top())
        {
            minY = pF->rect.top();
        }

        if (maxY < pF->rect.bottom())
        {
            maxY = pF->rect.bottom();
        }
    }

    return QRect(QPoint(minX, minY), QPoint(maxX, maxY));
}

QSize FrameCollector::adjustSize(bool bPowerOf2, bool bEqualSide)
{
    QRect miniRect = getSmallestRangeRect();
    QPoint transPt = -miniRect.topLeft();

    foreach (Frame* pFrame, m_frames) {
        pFrame->rect.translate(transPt);
    }

    QSize retSize = miniRect.size();
    if (bEqualSide)
    {
        int side = MAX(retSize.width(), retSize.height());
        retSize = QSize(side, side);
    }

    if (bPowerOf2)
    {
        int w = retSize.width();
        int h = retSize.height();

        w = (int)get2PowerValue((unsigned int) w);
        if (w == h)
        {
            h = w;
        }
        else
        {
            h = (int)get2PowerValue((unsigned int) h);
        }
        retSize =QSize(w, h);
    }

    setSize(retSize);
    return retSize;
}

void FrameCollector::addFrame(const QPixmap &image, const QString &name, const QRect &rect)
{
    Frame* pFrame = new Frame();
    pFrame->image = image;
    pFrame->name = name;
    pFrame->rect = rect;
    m_frames.append(pFrame);
}

void FrameCollector::addFrame(Frame *pFrame)
{
    if (pFrame)
    {
        m_frames.append(pFrame);
    }
}

const Frame* FrameCollector::getFrameByIndex(int index) const
{
    return valueAt(index);
}

void FrameCollector::removeFrameByIndex(int index)
{
    if (index < 0 || index >= m_frames.count())
    {
        return;
    }

    delete m_frames[index];
    m_frames.removeAt(index);
}

void FrameCollector::insertFrameBeforeIndex(int index, Frame* pFrame)
{
    if (index < 0 || index > m_frames.count() || NULL == pFrame)
    {
        return;
    }
    m_frames.insert(index, pFrame);
}

int FrameCollector::findFrameByName(const QString &name) const
{
    int nCount = m_frames.count();
    for (int i = 0; i < nCount; i++)
    {
        if (m_frames[i]->name == name)
        {
            return i;
        }
    }

    return -1;
}

int FrameCollector::findFrameByPosition(const QPoint& pos) const
{
    int nCount = m_frames.count();
    for (int i = 0; i < nCount; i++)
    {
        if (isPosInRect(pos, m_frames[i]->rect))
        {
            return i;
        }
    }

    return -1;
}
bool FrameCollector::findFramesInRect(const QRect &rect, QList<Frame*> *pResultFrames) const
{
    bool bRet = false;
    foreach (Frame* pFrame, m_frames) {
        if (isRectCross(pFrame->rect, rect))
        {
            if (!pResultFrames)
            {
                return true;
            }
            pResultFrames->append(pFrame);
            bRet = true;
        }
    }
    return bRet;
}

Frame* FrameCollector::valueAt(int index) const
{
    if (index < 0 || index >= m_frames.count())
    {
        //todo warning;
        return NULL;
    }
    return m_frames.value(index);
}

void FrameCollector::clear()
{
    foreach (Frame* pF, m_frames)
    {
        delete pF;
    }
    m_frames.clear();
}


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
