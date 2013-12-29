/*
 *  plist_rw version 1.0
 *  author by newma
 *  email to newma@live.cn
 */

#include <QtGui>
#include "core/frame.h"
#include "core/framecollector.h"
#include "util/util.h"

FrameCollector* FrameCollector::m_pInstance = NULL;
FrameCollector *FrameCollector::instance()
{
    if (!m_pInstance)
    {
        m_pInstance = new FrameCollector();
    }

    return m_pInstance;
}

FrameCollector::FrameCollector() : m_imageWidth(DEFAULT_IMAGE_SIZE), m_imageHeight(DEFAULT_IMAGE_SIZE), m_bIsModified(false)
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
        if (pF->bValid)
        {
            paint.drawPixmap(pF->rect, pF->image);
        }
    }

    return true;
}

QRect FrameCollector::getSmallestRangeRect() const
{
    if (m_frames.count() == 0)
    {
        return QRect(0, 0, m_imageWidth, m_imageHeight);
    }

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

        w = (int)getPowerOf2Value((unsigned int) w);
        if (w == h)
        {
            h = w;
        }
        else
        {
            h = (int)getPowerOf2Value((unsigned int) h);
        }
        retSize =QSize(w, h);
    }

    setSize(retSize);
    return retSize;
}

void FrameCollector::addFrame(const QPixmap &image, const QString &name, const QRect &rect, bool bValid)
{
    Frame* pFrame = new Frame();
    pFrame->image = image;
    pFrame->name = name;
    pFrame->rect = rect;
    pFrame->bValid = bValid;
    m_frames.append(pFrame);

    setModified(true);
    emit frameAdded(m_frames.count() - 1);
}

void FrameCollector::addFrame(Frame *pFrame)
{
    if (pFrame)
    {
        m_frames.append(pFrame);
        setModified(true);
        emit frameAdded(m_frames.count() - 1);
    }
}

const Frame* FrameCollector::getFrameByIndex(int index) const
{
    if (index < 0 || index >= m_frames.count())
    {
        //todo warning;
        return NULL;
    }
    return m_frames[index];
}

void FrameCollector::removeFrameByIndex(int index)
{
    if (index < 0 || index >= m_frames.count())
    {
        return;
    }

    Frame frame = *m_frames[index];
    delete m_frames[index];
    m_frames.removeAt(index);

    setModified(true);
    emit frameRemoved(index, frame);
}

void FrameCollector::insertFrameBeforeIndex(int index, Frame* pFrame)
{
    if (index < 0 || index > m_frames.count() || NULL == pFrame)
    {
        return;
    }
    m_frames.insert(index, pFrame);

    setModified(true);
    emit frameAdded(m_frames.count() - 1);
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
        if (m_frames[i]->bValid && isPosInRect(pos, m_frames[i]->rect))
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
        if (pFrame->bValid && isRectCross(pFrame->rect, rect))
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

void FrameCollector::updateFrame(int index, const QPixmap &image, const QString &name, const QRect &rect, bool bValid)
{
    if (index < 0 || index >= m_frames.count())
    {
        return;
    }
    m_frames[index]->image = image;
    m_frames[index]->name = name;
    m_frames[index]->rect = rect;
    m_frames[index]->bValid = bValid;
    setModified(true);
    emit frameChanged(index);
}

void FrameCollector::updateFrame(int index, const Frame *pFrame)
{
    if (index < 0 || index >= m_frames.count())
    {
        return;
    }
    *m_frames[index] = *pFrame;
    setModified(true);
    emit frameChanged(index);
}

void FrameCollector::updateFrameName(int index, const QString &name)
{
    if (index < 0 || index >= m_frames.count())
    {
        return;
    }
    m_frames[index]->name = name;

    setModified(true);
    emit frameChanged(index);
}

void FrameCollector::updateFrameImage(int index, const QPixmap &image)
{
    if (index < 0 || index >= m_frames.count())
    {
        return;
    }
    m_frames[index]->image = image;

    setModified(true);
    emit frameChanged(index);
}

void FrameCollector::updateFrameRect(int index, const QRect &rect)
{
    if (index < 0 || index >= m_frames.count())
    {
        return;
    }
    m_frames[index]->rect = rect;

    setModified(true);
    emit frameChanged(index);
}

void FrameCollector::updateFrameValid(int index, bool bValid)
{
    if (index < 0 || index >= m_frames.count())
    {
        return;
    }
    m_frames[index]->bValid = bValid;

    setModified(true);
    emit frameChanged(index);
}

void FrameCollector::clear()
{
    m_bIsModified = false;
    foreach (Frame* pF, m_frames)
    {
        delete pF;
    }
    m_frames.clear();
}
