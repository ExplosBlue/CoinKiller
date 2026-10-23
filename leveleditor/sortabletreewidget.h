#ifndef SORTABLETREEWIDGET_H
#define SORTABLETREEWIDGET_H

#include <QTreeWidget>
#include <QVariant>

class SortableTreeItem : public QTreeWidgetItem
{
public:
    using QTreeWidgetItem::QTreeWidgetItem;

    void setSortKey(int column, const QVariant& key) { setData(column, Qt::UserRole + 1, key); }

    bool operator<(const QTreeWidgetItem& other) const override;
};

class SortableTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit SortableTreeWidget(QWidget* parent = nullptr);

    void setSortableColumns(const QList<int>& columns);
    void setFlexibleColumn(int column);
    void resetColumnSizing();
    void resort();

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void handleHeaderClicked(int section);

private:
    void fitColumnsToContents();

    QList<int> sortableColumns;
    int flexibleColumn = 0;
    bool columnsFitted = false;
    int lastViewportWidth = 0;
    int sortColumn = 0;
    Qt::SortOrder sortOrder = Qt::AscendingOrder;
};

#endif // SORTABLETREEWIDGET_H
