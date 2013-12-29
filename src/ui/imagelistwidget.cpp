/*
 *  plist_rw version 1.0
 *  author by newma
 *  email to newma@live.cn
 */
#include <QtGui>
#include <QPixmap>
#include "imagelistwidget.h"

#define MIME_DATA_TYPE "image/plist-rw"

ImageListWidget::ImageListWidget(QWidget *parent) :
    QListWidget(parent)
{
    setDragEnabled(true);
    setViewMode(QListView::ListMode);
    setIconSize(QSize(icon_size, icon_size));
    setSpacing(item_space);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setSortingEnabled(true);
}

void ImageListWidget::initWithDirectory(QString& directory)
{
    this->clear();

    QDir dir(directory);

    dir.setFilter(QDir::Files | QDir::NoSymLinks);

    QStringList filters;
    filters << "*.png" << "*.jpeg" << "*.bmp";
    dir.setNameFilters(filters);

    QFileInfoList list = dir.entryInfoList();
    foreach (QFileInfo file, list) {
        QPixmap image;
        if (image.load(file.absoluteFilePath()))
        {
            addImage(image, file.fileName());
        }
    }
}

void ImageListWidget::addImage(QPixmap& image, QString name)
{
    QListWidgetItem* pItem = new QListWidgetItem();
    pItem->setIcon(QIcon(image));
    pItem->setText(name);

    pItem->setData(Qt::UserRole, QVariant(image));
    pItem->setData(Qt::UserRole + 1, name);

    pItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable
                        | Qt::ItemIsDragEnabled);
    addItem(pItem);
    setCurrentItem(pItem);
}

void ImageListWidget::dragEnterEvent(QDragEnterEvent *event)
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

void ImageListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat(MIME_DATA_TYPE))
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void ImageListWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat(MIME_DATA_TYPE)) {
        QByteArray pieceData = event->mimeData()->data(MIME_DATA_TYPE);
        QDataStream dataStream(&pieceData, QIODevice::ReadOnly);
        QPixmap pixmap;
        QString name;
        int index = -1;
        dataStream >> pixmap >> name >> index;

        addImage(pixmap, name);

        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void ImageListWidget::startDrag(Qt::DropActions supportedActions)
{
    QListWidgetItem *item = currentItem();

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    QPixmap pixmap = qvariant_cast<QPixmap>(item->data(Qt::UserRole));
    QString name = qvariant_cast<QString>(item->data(Qt::UserRole + 1));
    int index = -1;
    dataStream << pixmap << name << index;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData(MIME_DATA_TYPE, itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));
    drag->setPixmap(pixmap);

    if (drag->exec(Qt::MoveAction) == Qt::MoveAction)
        delete takeItem(row(item));
}
