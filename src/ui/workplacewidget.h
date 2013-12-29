/*
 *  workplacewidget.h
 *  plist_rw version 1.0
 *  author by newma
 *  email to newma@live.cn
 */
#ifndef WORKPLACEWIDGET_H
#define WORKPLACEWIDGET_H

#include <QPixmap>
#include <QList>
#include <QWidget>

class Frame;
class FrameCollector;

class WorkPlaceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WorkPlaceWidget(FrameCollector* pCollector = NULL, QWidget *parent = 0);

    FrameCollector* getCollector() const;
    void setCollector(FrameCollector* pCollector);

public slots:
    void updateContent();
    void updateContentSize(QSize size);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    FrameCollector* m_pCollector;
    QRect m_highlightedRect;
};

#endif // WORKPLACEWIDGET_H
