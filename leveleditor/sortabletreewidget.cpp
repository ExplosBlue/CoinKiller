#include "sortabletreewidget.h"

#include <QHeaderView>
#include <QMetaType>
#include <QResizeEvent>

bool SortableTreeItem::operator<(const QTreeWidgetItem& other) const
{
    int column = treeWidget() ? treeWidget()->sortColumn() : 0;

    QVariant a = data(column, Qt::UserRole + 1);
    QVariant b = other.data(column, Qt::UserRole + 1);

    if (a.typeId() == QMetaType::Double && b.typeId() == QMetaType::Double)
        return a.toDouble() < b.toDouble();

    return a.toString() < b.toString();
}

SortableTreeWidget::SortableTreeWidget(QWidget* parent) :
    QTreeWidget(parent) {

    setRootIsDecorated(false);
    setSortingEnabled(false);
    header()->setStretchLastSection(true);
    header()->setSectionsClickable(true);
    header()->setSortIndicatorShown(true);
    header()->setSortIndicator(sortColumn, sortOrder);
    connect(header(), &QHeaderView::sectionClicked, this, &SortableTreeWidget::handleHeaderClicked);
}

void SortableTreeWidget::setSortableColumns(const QList<int>& columns)
{
    sortableColumns = columns;
}

void SortableTreeWidget::setFlexibleColumn(int column)
{
    flexibleColumn = column;
}

void SortableTreeWidget::resetColumnSizing()
{
    columnsFitted = false;
}

void SortableTreeWidget::fitColumnsToContents()
{
    if (columnsFitted || !isVisible() || topLevelItemCount() == 0 || columnCount() == 0 || viewport()->width() <= 0)
        return;

    bool stretch = header()->stretchLastSection();
    header()->setStretchLastSection(false);
    header()->resizeSections(QHeaderView::ResizeToContents);

    int last = columnCount() - 1;
    header()->resizeSection(last, header()->sectionSize(last) + 24);

    int total = 0;
    for (int i = 0; i < columnCount(); i++)
        total += header()->sectionSize(i);

    int flex = flexibleColumn;
    if (flex < 0 || flex >= columnCount() - 1)
        flex = 0;

    int extra = viewport()->width() - total;
    if (extra > 0 && columnCount() > 1 && flex < columnCount() - 1)
        header()->resizeSection(flex, header()->sectionSize(flex) + extra);

    header()->setStretchLastSection(stretch);
    columnsFitted = true;
    lastViewportWidth = viewport()->width();
}

void SortableTreeWidget::resort()
{
    if (sortColumn < 0 || sortColumn >= columnCount())
    {
        sortColumn = 0;
        sortOrder = Qt::AscendingOrder;
    }

    fitColumnsToContents();

    header()->setSortIndicator(sortColumn, sortOrder);
    sortItems(sortColumn, sortOrder);
}

void SortableTreeWidget::resizeEvent(QResizeEvent* event)
{
    QTreeWidget::resizeEvent(event);

    if (!columnsFitted)
    {
        fitColumnsToContents();
        return;
    }

    int delta = viewport()->width() - lastViewportWidth;
    lastViewportWidth = viewport()->width();

    int flex = flexibleColumn;
    if (flex < 0 || flex >= columnCount() - 1)
        flex = 0;

    int total = 0;
    for (int i = 0; i < columnCount(); i++)
        total += header()->sectionSize(i);

    if (delta != 0 && total <= viewport()->width() && columnCount() > 1 && flex < columnCount() - 1)
    {
        int width = header()->sectionSize(flex) + delta;
        if (width >= header()->minimumSectionSize())
            header()->resizeSection(flex, width);
    }
}

void SortableTreeWidget::handleHeaderClicked(int section)
{
    if (!sortableColumns.contains(section))
    {
        header()->setSortIndicator(sortColumn, sortOrder);
        return;
    }

    if (section == sortColumn)
        sortOrder = (sortOrder == Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
    else
    {
        sortColumn = section;
        sortOrder = Qt::AscendingOrder;
    }

    header()->setSortIndicator(sortColumn, sortOrder);
    sortItems(sortColumn, sortOrder);
}
