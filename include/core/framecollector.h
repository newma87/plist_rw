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

class FrameCollector : public QObject
{
    Q_OBJECT
private:
    FrameCollector();
public:
    static FrameCollector* instance();
    ~FrameCollector();

signals:
    void frameChanged(int index);
    void frameAdded(int index);
    void frameRemoved(int index, const Frame& frame);
    void sizeChanged(QSize newSize);
    void contentWasModified();

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
    void addFrame(const QPixmap& image, const QString& name, const QRect& rect, bool bValid = true);
    void addFrame(Frame* pFrame);

    const Frame* getFrameByIndex(int index) const;
    void removeFrameByIndex(int index);
    void insertFrameBeforeIndex(int index, Frame* pFrame);

    int findFrameByName(const QString& name) const;
    int findFrameByPosition(const QPoint &pos) const;
    bool findFramesInRect(const QRect& rect, QList<Frame*>* pResultFrames = NULL) const;

    void updateFrame(int index, const QPixmap& image, const QString& name, const QRect& rect, bool bValid);
    void updateFrame(int index, const Frame* pFrame);
    void updateFrameName(int index, const QString& name);
    void updateFrameImage(int index, const QPixmap& image);
    void updateFrameRect(int index, const QRect& rect);
    void updateFrameValid(int index, bool bValid);

    // clear all the stuff in this collector
    void clear();
public:
    typedef QList<Frame*>::const_iterator const_iterator;

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

        setModified(true);
        emit sizeChanged(QSize(m_imageWidth, m_imageHeight));
    }

    bool isModified() const
    {
        return m_bIsModified;
    }

    void setModified(bool val)
    {
        m_bIsModified = val;
        emit contentWasModified();
    }

private:
    static FrameCollector* m_pInstance;

    int m_imageWidth;
    int m_imageHeight;
    bool m_bIsModified;

    QList<Frame*> m_frames;
};

#endif // FRAMECOLLECTOR_H
