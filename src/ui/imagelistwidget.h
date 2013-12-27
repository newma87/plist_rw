/*
 *  imagelistwidget.h
 *  plist_rw version 1.0
 *  author by newma
 *  email to newma@live.cn
 */
#ifndef IMAGELISTWIDGET_H
#define IMAGELISTWIDGET_H

#include <QPixmap>
#include <QListWidget>

class ImageListWidget : public QListWidget
{
    Q_OBJECT

public:
    enum { icon_size = 32, item_space = 2 };

public:
    explicit ImageListWidget(QWidget *parent = 0);
    
    void initWithDirectory(QString& directory);
    void addImage(QPixmap& image, QString name);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void startDrag(Qt::DropActions supportedActions);
};

#endif // IMAGELISTWIDGET_H
