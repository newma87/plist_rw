/*
 *  plist_rw version 1.0
 *  author by newma
 *  email to newma@live.cn
 */
#include <QHeaderView>
#include "attributewidget.h"
#include "core/frame.h"
#include "core/framecollector.h"

AttributeWidget::AttributeWidget(FrameCollector* pCollector, QWidget *parent) :
    QTreeWidget(parent), m_pCollector(pCollector)
{
    QStringList labels;
    labels << tr("Property") << tr("Value");

    header()->setResizeMode(QHeaderView::ResizeToContents);
    setHeaderLabels(labels);

    setSelectionMode(SingleSelection);
}

void AttributeWidget::refresh()
{
    clear();
    m_hashElement.clear();
    m_frameElement.clear();

    disconnect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
               this, SLOT(onItemValuedChanged(QTreeWidgetItem*,int)));

    int count = m_pCollector->count();
    for (int index = 0; index < count; index++)
    {
        parseFrameElement(index);
    }

    connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
            this, SLOT(onItemValuedChanged(QTreeWidgetItem*,int)));
}

void AttributeWidget::onFrameItemSelected(int index)
{
    if (index >= 0 && index < m_frameElement.size())
    {
        m_frameElement[index]->setExpanded(true);
        m_frameElement[index]->setSelected(true);
    }
}

void AttributeWidget::onItemValuedChanged(QTreeWidgetItem *item, int column)
{
    QVariant v = item->data(0, Qt::UserRole);
    int type = v.toInt();
    int index = m_hashElement[item];
    if (column == 0)
    {
        if (type != name_item)
        {
            return;
        }
        m_pCollector->updateFrameName(index, item->text(column));
    }
    else
    {
        int value =  item->text(column).toInt();
        QRect rect = m_pCollector->getFrameByIndex(index)->rect;
        switch (type)
        {
        case x_item:
            rect.moveTo(value, rect.y());
            break;
        case y_item:
            rect.moveTo(rect.x(), value);
            break;
        case width_item:
            rect.setWidth(value);
            break;
        case height_item:
            rect.setHeight(value);
            break;
        default:
            return;
        }

        m_pCollector->updateFrameRect(index, rect);
     }
}

void AttributeWidget::parseFrameElement( int index, QTreeWidgetItem *parentItem)
{
    QTreeWidgetItem *pItem = createItem(index, parentItem);
    const Frame* pFrame = m_pCollector->getFrameByIndex(index);

    // name
    pItem->setFlags(pItem->flags() | Qt::ItemIsEditable);
    pItem->setIcon(0, QIcon(pFrame->image));
    pItem->setText(0, pFrame->name);
    pItem->setDisabled(!pFrame->bValid);

    // x
    QTreeWidgetItem *pOrgXItem = createItem(index, pItem);
    pOrgXItem->setFlags(pItem->flags() | Qt::ItemIsEditable);
    pOrgXItem->setText(0, QString(tr("x")));
    pOrgXItem->setText(1, QString::number(pFrame->rect.x()));
    pOrgXItem->setDisabled(!pFrame->bValid);

    // y
    QTreeWidgetItem *pOrgYItem = createItem(index, pItem);
    pOrgYItem->setFlags(pItem->flags() | Qt::ItemIsEditable);
    pOrgYItem->setText(0, QString(tr("y")));
    pOrgYItem->setText(1, QString::number(pFrame->rect.y()));
    pOrgYItem->setDisabled(!pFrame->bValid);

    // width
    QTreeWidgetItem *pWidthItem = createItem(index, pItem);
    pWidthItem->setFlags(pItem->flags() | Qt::ItemIsEditable);
    pWidthItem->setText(0, QString(tr("width")));
    pWidthItem->setText(1, QString::number(pFrame->rect.width()));
    pWidthItem->setDisabled(!pFrame->bValid);

    // height
    QTreeWidgetItem *pHeightItem = createItem(index, pItem);
    pHeightItem->setFlags(pItem->flags() | Qt::ItemIsEditable);
    pHeightItem->setText(0, QString(tr("height")));
    pHeightItem->setText(1, QString::number(pFrame->rect.height()));
    pHeightItem->setDisabled(!pFrame->bValid);

    pItem->setData(0, Qt::UserRole, QVariant(name_item));
    pOrgXItem->setData(0, Qt::UserRole, QVariant(x_item));
    pOrgYItem->setData(0, Qt::UserRole, QVariant(y_item));
    pWidthItem->setData(0, Qt::UserRole, QVariant(width_item));
    pHeightItem->setData(0 ,Qt::UserRole, QVariant(height_item));

    m_frameElement.append(pItem);
}

QTreeWidgetItem *AttributeWidget::createItem(int index, QTreeWidgetItem *parentItem)
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

    m_hashElement.insert(pItem, index);
    return pItem;
}

void AttributeWidget::updateFrameElement(int index)
{
    if (index < 0 || index >= m_frameElement.count() || index >= m_pCollector->count())
    {
        return;
    }

    disconnect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
               this, SLOT(onItemValuedChanged(QTreeWidgetItem*,int)));

    QTreeWidgetItem* pItem = m_frameElement[index];
    const Frame* pFrame = m_pCollector->getFrameByIndex(index);

    pItem->setIcon(0, QIcon(pFrame->image));
    pItem->setText(0, pFrame->name);
    pItem->setDisabled(!pFrame->bValid);

    int childCount = pItem->childCount();
    for (int i = 0; i < childCount; i++)
    {
        QTreeWidgetItem* pChild = pItem->child(i);
        int type = pChild->data(0, Qt::UserRole).toInt();
        switch (type)
        {
        case x_item:
            pChild->setText(1, QString::number(pFrame->rect.x()));
            break;
        case y_item:
            pChild->setText(1, QString::number(pFrame->rect.y()));
            break;
        case width_item:
            pChild->setText(1, QString::number(pFrame->rect.width()));
            break;
        case height_item:
            pChild->setText(1, QString::number(pFrame->rect.height()));
            break;
        default:
            break;
        }
        pChild->setDisabled(!pFrame->bValid);
    }

    connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
            this, SLOT(onItemValuedChanged(QTreeWidgetItem*,int)));
}


