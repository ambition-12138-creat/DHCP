#include "iptablewidget.h"
#include <QHeaderView>

IPTableWidget::IPTableWidget(QWidget *parent)
    : QTableWidget(parent)
{
    setupTable();
}

void IPTableWidget::setupTable()
{
    setColumnCount(4);
    setHorizontalHeaderLabels({"IP 地址", "状态", "绑定客户端", "分配时间"});
    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setAlternatingRowColors(true);
}

void IPTableWidget::refresh(const QVector<IPEntry> &entries)
{
    setRowCount(entries.size());
    for (int i = 0; i < entries.size(); ++i) {
        const auto &entry = entries[i];
        setItem(i, 0, new QTableWidgetItem(entry.ipAddress()));
        setItem(i, 1, new QTableWidgetItem(entry.isAllocated() ? "已分配" : "可用"));
        setItem(i, 2, new QTableWidgetItem(entry.isAllocated() ? entry.clientMac() : "-"));
        setItem(i, 3, new QTableWidgetItem(
            entry.isAllocated() ? entry.allocatedTime().toString("hh:mm:ss") : "-"));
    }
}

void IPTableWidget::updateEntry(const IPEntry &entry)
{
    for (int i = 0; i < rowCount(); ++i) {
        if (item(i, 0)->text() == entry.ipAddress()) {
            item(i, 1)->setText(entry.isAllocated() ? "已分配" : "可用");
            item(i, 2)->setText(entry.isAllocated() ? entry.clientMac() : "-");
            item(i, 3)->setText(
                entry.isAllocated() ? entry.allocatedTime().toString("hh:mm:ss") : "-");
            break;
        }
    }
}
