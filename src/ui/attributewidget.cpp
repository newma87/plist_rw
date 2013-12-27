/*
 *  plist_rw version 1.0
 *  author by newma
 *  email to newma@live.cn
 */
#include <QHeaderView>
#include "attributewidget.h"
#include "frame.h"
#include "framecollector.h"

AttributeWidget::AttributeWidget(FrameCollector* pCollector, QWidget *parent) :
    QTreeWidget(parent), m_pCollector(pCollector)
{
    QStringList labels;
    labels << tr("Property") << tr("Value");

    header()->setResizeMode(QHeaderView::ResizeToContents);
    setHeaderLabels(labels);
}

void AttributeWidget::refresh()
{
    clear();
    m_hashElement.clear();

    disconnect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
               this, SLOT(updateDomElement(QTreeWidgetItem*,int)));

    FrameCollector::iterator it = m_pCollector->begin();
    for (; it != m_pCollector->end(); ++it)
    {
        parseFrameElement(*it);
    }

    connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
            this, SLOT(updateDomElement(QTreeWidgetItem*,int)));
}

void AttributeWidget::updateDomElement(QTreeWidgetItem *item, int column)
{
    QVariant v = item->data(0, Qt::UserRole);
    int type = v.toInt();
    Frame* pFrame = m_hashElement[item];
    if (column == 0)
    {
        if (type != image_item)
        {
            return;
        }

        pFrame->name = item->text(column);
        emit contentWasModified();
    }
    else
    {
        int value =  item->text(column).toInt();
        switch (type)
        {
        case x_item:
            pFrame->rect.moveTo(value, pFrame->rect.y());
            break;
        case y_item:
            pFrame->rect.moveTo(pFrame->rect.x(), value);
            break;
        case width_item:
            pFrame->rect.setWidth(value);
            break;
        case height_item:
            pFrame->rect.setHeight(value);
            break;
        default:
            return;
        }
        emit contentWasModified();
    }
}

void AttributeWidget::parseFrameElement( Frame *pFrame, QTreeWidgetItem *parentItem)
{
    QTreeWidgetItem *pImage = createItem(pFrame, parentItem);
    // name
    pImage->setFlags(pImage->flags() | Qt::ItemIsEditable);
    pImage->setIcon(0, QIcon(pFrame->image));
    pImage->setText(0, pFrame->name);

    // x
    QTreeWidgetItem *pOrgXItem = createItem(pFrame, pImage);
    pOrgXItem->setFlags(pImage->flags() | Qt::ItemIsEditable);
    pOrgXItem->setText(0, QString(tr("x")));
    pOrgXItem->setText(1, QString::number(pFrame->rect.x()));

    // y
    QTreeWidgetItem *pOrgYItem = createItem(pFrame, pImage);
    pOrgYItem->setFlags(pImage->flags() | Qt::ItemIsEditable);
    pOrgYItem->setText(0, QString(tr("y")));
    pOrgYItem->setText(1, QString::number(pFrame->rect.y()));

    // width
    QTreeWidgetItem *pWidthItem = createItem(pFrame, pImage);
    pWidthItem->setFlags(pImage->flags() | Qt::ItemIsEditable);
    pWidthItem->setText(0, QString(tr("width")));
    pWidthItem->setText(1, QString::number(pFrame->rect.width()));

    // height
    QTreeWidgetItem *pHeightItem = createItem(pFrame, pImage);
    pHeightItem->setFlags(pImage->flags() | Qt::ItemIsEditable);
    pHeightItem->setText(0, QString(tr("height")));
    pHeightItem->setText(1, QString::number(pFrame->rect.height()));

    pImage->setExpanded(true);
    pImage->setData(0, Qt::UserRole, QVariant(image_item));
    pOrgXItem->setData(0, Qt::UserRole, QVariant(x_item));
    pOrgYItem->setData(0, Qt::UserRole, QVariant(y_item));
    pWidthItem->setData(0, Qt::UserRole, QVariant(width_item));
    pHeightItem->setData(0 ,Qt::UserRole, QVariant(height_item));

}

QTreeWidgetItem *AttributeWidget::createItem(Frame *pFrame, QTreeWidgetItem *parentItem)
{
    QTreeWidgetItem* pItem;
    if (parentItem)
    {
        pItem = new QTreeWidgetItem(parentItem);
    }
    else
    {
        pItem = new QTreeWidgetItem(this);
    }

    m_hashElement.insert(pItem, pFrame);
    return pItem;
}


