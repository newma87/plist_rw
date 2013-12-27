/*
 *  attributewidget.h
 *  plist_rw version 1.0
 *  author by newma
 *  email to newma@live.cn
 */
#ifndef ATTRIBUTEWIDGET_H
#define ATTRIBUTEWIDGET_H

#include <QHash>
#include <QTreeWidget>

class Frame;
class FrameCollector;

class AttributeWidget : public QTreeWidget
{
    enum { image_item, x_item, y_item, width_item, height_item };
    Q_OBJECT
public:
    explicit AttributeWidget(FrameCollector* pCollector, QWidget *parent = NULL);

public slots:
    void updateDomElement(QTreeWidgetItem *item, int column);
    void refresh();

signals:
    void contentWasModified();

private:
    void parseFrameElement(Frame* pFrame, QTreeWidgetItem *parentItem = NULL);
    QTreeWidgetItem* createItem(Frame* pFrame, QTreeWidgetItem* parentItem);

private:
    FrameCollector* m_pCollector;
    QHash<QTreeWidgetItem*, Frame*> m_hashElement;
};

#endif // ATTRIBUTEWIDGET_H
