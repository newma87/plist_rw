/*
 *  plist_rw version 1.0
 *  author by newma
 *  email to newma@live.cn
 */
#include <QtGui>
#include "core/frame.h"
#include "core/framecollector.h"
#include "workplacewidget.h"
#include "core/alignalgorithm.h"

#define MIME_DATA_TYPE "image/plist-rw"

WorkPlaceWidget::WorkPlaceWidget(FrameCollector* pCollector, QWidget *parent) :
    QWidget(parent), m_pCollector(pCollector)
{
    setAcceptDrops(true);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    int w = m_pCollector->getSize().width();
    int h = m_pCollector->getSize().height();
    setMinimumSize(QSize(w + 1, h + 1));
}

FrameCollector *WorkPlaceWidget::getCollector() const
{
    return m_pCollector;
}

void WorkPlaceWidget::setCollector(FrameCollector *pCollector)
{
    m_pCollector = pCollector;
}

void WorkPlaceWidget::updateContent()
{
    update(QRect(QPoint(0, 0), m_pCollector->getSize()));
}

void WorkPlaceWidget::updateContentSize(QSize size)
{
    QSize contentSize = m_pCollector->getSize() + QSize(1, 1);
    update();
    setMinimumSize(contentSize);
}

void WorkPlaceWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(MIME_DATA_TYPE))
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void WorkPlaceWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    QRect updateRect = m_highlightedRect;
    m_highlightedRect = QRect();
    update(updateRect);
    event->accept();
}

void WorkPlaceWidget::dragMoveEvent(QDragMoveEvent *event)
{
    QRect oldHightLighted = m_highlightedRect;
    if (event->mimeData()->hasFormat(MIME_DATA_TYPE))
    {
        QByteArray imageData = event->mimeData()->data(MIME_DATA_TYPE);
        QDataStream dataStream(&imageData, QIODevice::ReadOnly);

        QPixmap image;
        QString name;
        dataStream >> image >> name;

        m_highlightedRect = NormalAlignAlgorithm::findPropertyPlace(*m_pCollector, event->pos(), image.size());

        event->setDropAction(Qt::MoveAction);
        event->accept();
    } else {
        m_highlightedRect = QRect();
        event->ignore();
    }

    if (m_highlightedRect.isValid())
    {
        oldHightLighted.unite(m_highlightedRect);
    }
    update(oldHightLighted);
}

void WorkPlaceWidget::dropEvent(QDropEvent *event)
{
    update(m_highlightedRect);
    m_highlightedRect = QRect();

    do {
        if (!event->mimeData()->hasFormat(MIME_DATA_TYPE))
        {
            break;
        }
        QByteArray imageData = event->mimeData()->data(MIME_DATA_TYPE);
        QDataStream dataStream(&imageData, QIODevice::ReadOnly);

        QPixmap image;
        QString name;
        int index;

        dataStream >> image >> name >> index;

        QRect target = NormalAlignAlgorithm::findPropertyPlace(*m_pCollector, event->pos(), image.size());

        if (!target.isValid())
        {
            break;
        }
        if (event->source() != this)
        {
            m_pCollector->addFrame(image, name, target, true);
        }
        else
        {
            const Frame* pFrame = m_pCollector->getFrameByIndex(index);
            m_pCollector->updateFrame(index, pFrame->image, pFrame->name, target, true);
       }

        event->setDropAction(Qt::MoveAction);
        event->accept();

        return;
    }while(0);

    event->ignore();
}

void WorkPlaceWidget::mousePressEvent(QMouseEvent *event)
{
    int found = m_pCollector->findFrameByPosition(event->pos());

    if (found == -1)
        return;

    const Frame* pFrame = m_pCollector->getFrameByIndex(found);
    QString name = pFrame->name;
    QPixmap pixmap = pFrame->image;
    QRect rect = pFrame->rect;
    m_pCollector->updateFrameValid(found, false);

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);

    dataStream << pixmap << name << found;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData(MIME_DATA_TYPE, itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setHotSpot(rect.center() - rect.topLeft());
    drag->setPixmap(pixmap);

    if (!(drag->exec(Qt::MoveAction) == Qt::MoveAction))
    {
        m_pCollector->updateFrameValid(found, true);
    }
    else
    {
        if (drag->target() != this)
        {
            m_pCollector->removeFrameByIndex(found);
        }
    }
}

void WorkPlaceWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(event->rect(), Qt::gray);

    if (m_highlightedRect.isValid()) {
        painter.setBrush(QColor("#ccffff"));
        painter.setPen(Qt::NoPen);
        painter.drawRect(m_highlightedRect.adjusted(0, 0, -1, -1));
    }
    QBrush oldBrush = painter.brush();
    painter.setBrush(Qt::NoBrush);
    QPen pen = QPen(QColor(Qt::red));
    pen.setWidth(1);
    pen.setStyle(Qt::SolidLine);
    QPen oldPen = painter.pen();
    painter.setPen(pen);
    painter.drawRect(QRect(QPoint(0, 0), m_pCollector->getSize()));
    painter.setBrush(oldBrush);
    painter.setPen(oldPen);

    FrameCollector::const_iterator it = m_pCollector->constBegin();
    for (; it < m_pCollector->constEnd(); ++it)
    {
        const Frame* pFrame = *it;
        if (pFrame->bValid)
        {
            painter.drawPixmap(pFrame->rect, pFrame->image);
        }
    }
}
