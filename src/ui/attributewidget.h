/*
 *  attributewidget.h
 *  plist_rw version 1.0
 *  author by newma
 *  email to newma@live.cn
 */
#ifndef ATTRIBUTEWIDGET_H
#define ATTRIBUTEWIDGET_H

#include <QHash>
#include <QList>
#include <QTreeWidget>

class Frame;
class FrameCollector;

class AttributeWidget : public QTreeWidget
{
    enum { name_item, x_item, y_item, width_item, height_item };
    Q_OBJECT
public:
    explicit AttributeWidget(FrameCollector* pCollector, QWidget *parent = NULL);

public slots:
    void onFrameItemSelected(int index);
    void onItemValuedChanged(QTreeWidgetItem *item, int column);
    void refresh();
    void updateFrameElement(int index);

private:
    void parseFrameElement(int frameIndex, QTreeWidgetItem *parentItem = NULL);
    QTreeWidgetItem* createItem(int frameIndex, QTreeWidgetItem* parentItem);

private:
    FrameCollector* m_pCollector;
    QHash<QTreeWidgetItem*, int> m_hashElement;
    QList<QTreeWidgetItem*> m_frameElement;
};

#endif // ATTRIBUTEWIDGET_H
