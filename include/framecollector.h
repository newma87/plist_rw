/*
 *  framecollector.h
 *  plist_rw version 1.0
 *  author by newma
 *  email to newma@live.cn
 */
#ifndef FRAMECOLLECTOR_H
#define FRAMECOLLECTOR_H

#include <QList>
#include <QImage>
#include <QDomDocument>

struct Frame;

class FrameCollector
{
public:
    FrameCollector();
    ~FrameCollector();

public:
    static const int DEFAULT_IMAGE_SIZE = 1024;

    // get an image that already draw all of the stuff on it, the image's size is getSize();
    bool getImage(QImage** ppImage) const;

    // get smallest rect that contain all the stuff
    QRect getSmallestRangeRect() const;

    // call this functoin will change the collector's size and all the stuff's position in collector.
    // bPowerOf2: the result size is two to power of n.
    // bEqualSize: final rect is square.
    QSize adjustSize(bool bPowerOf2 = true, bool bEqualSide = true);

public:
    void addFrame(const QPixmap& image, const QString& name, const QRect& rect);
    void addFrame(Frame* pFrame);

    const Frame* getFrameByIndex(int index) const;
    void removeFrameByIndex(int index);
    void insertFrameBeforeIndex(int index, Frame* pFrame);

    int findFrameByName(const QString& name) const;
    int findFrameByPosition(const QPoint &pos) const;
    bool findFramesInRect(const QRect& rect, QList<Frame*>* pResultFrames = NULL) const;

    Frame* valueAt(int index) const; // only used when you want to chang the value in list

    // clear all the stuff in this collector
    void clear();
public:
    typedef QList<Frame*>::iterator iterator;
    typedef QList<Frame*>::const_iterator const_iterator;
    iterator begin()
    {
        return m_frames.begin();
    }
    iterator end()
    {
        return m_frames.end();
    }

    const_iterator constBegin() const
    {
        return m_frames.constBegin();
    }
    const_iterator constEnd() const
    {
        return m_frames.constEnd();
    }

    int count() const
    {
        return m_frames.count();
    }

    QSize getSize() const
    {
        return QSize(m_imageWidth, m_imageHeight);
    }

    void setSize(const QSize& size)
    {
        m_imageWidth = size.width();
        m_imageHeight = size.height();
    }

private:
    int m_imageWidth;
    int m_imageHeight;

    QList<Frame*> m_frames;
};

class NormalAlignAlgorithm
{
public:
    enum {
        algin_lenth = 3   // align base lenth (pixel)
    };

    static QRect findPropertyPlace(const FrameCollector& collector, const QPoint& mousePos, const QSize& imageSize);
    static QRect findPropertyPlace(const FrameCollector& collector, const QRect& imageRect);

protected:
    static QRect adjustRectInRange(const QRect& target, const QRect& ragneRect);
    static int nearAlign(int l1, int l2, int base, int* translate_dist);
};

#endif // FRAMECOLLECTOR_H
