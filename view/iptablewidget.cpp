#include "iptablewidget.h"
#include <QHeaderView>
#include <QColor>

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
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setAlternatingRowColors(true);
}

void IPTableWidget::refresh(const QVector<IPEntry> &entries)
{
    setRowCount(entries.size());
    for (int i = 0; i < entries.size(); ++i) {
        const auto &entry = entries[i];
        bool allocated = entry.isAllocated();

        auto *ipItem    = new QTableWidgetItem(entry.ipAddress());
        auto *statItem  = new QTableWidgetItem(allocated ? "已分配" : "可用");
        auto *macItem   = new QTableWidgetItem(allocated ? entry.clientMac() : "-");
        auto *timeItem  = new QTableWidgetItem(
            allocated ? entry.allocatedTime().toString("hh:mm:ss") : "-");

        if (allocated) {
            // blue tint for allocated
            QColor bg("#1a1a2e");
            QColor fg("#89b4fa");
            ipItem  ->setBackground(bg); ipItem  ->setForeground(fg);
            statItem->setBackground(bg); statItem->setForeground(fg);
            macItem ->setBackground(bg); macItem ->setForeground(fg);
            timeItem->setBackground(bg); timeItem->setForeground(fg);
        } else {
            // green tint for available
            QColor bg("#1a1e1a");
            QColor fg("#a6e3a1");
            ipItem  ->setBackground(bg); ipItem  ->setForeground(fg);
            statItem->setBackground(bg); statItem->setForeground(fg);
            macItem ->setBackground(bg); macItem ->setForeground(fg);
            timeItem->setBackground(bg); timeItem->setForeground(fg);
        }

        setItem(i, 0, ipItem);
        setItem(i, 1, statItem);
        setItem(i, 2, macItem);
        setItem(i, 3, timeItem);
    }
}

void IPTableWidget::updateEntry(const IPEntry &entry)
{
    for (int i = 0; i < rowCount(); ++i) {
        if (item(i, 0)->text() == entry.ipAddress()) {
            bool allocated = entry.isAllocated();
            item(i, 1)->setText(allocated ? "已分配" : "可用");
            item(i, 2)->setText(allocated ? entry.clientMac() : "-");
            item(i, 3)->setText(
                allocated ? entry.allocatedTime().toString("hh:mm:ss") : "-");

            QColor bg(allocated ? "#1a1a2e" : "#1a1e1a");
            QColor fg(allocated ? "#89b4fa" : "#a6e3a1");
            for (int c = 0; c < columnCount(); ++c) {
                item(i, c)->setBackground(bg);
                item(i, c)->setForeground(fg);
            }
            break;
        }
    }
}
