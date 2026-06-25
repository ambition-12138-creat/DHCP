#include "leasetablewidget.h"
#include <QHeaderView>
#include <QColor>

LeaseTableWidget::LeaseTableWidget(QWidget *parent)
    : QTableWidget(parent)
{
    setupTable();
}

void LeaseTableWidget::setupTable()
{
    setColumnCount(5);
    setHorizontalHeaderLabels({"Client MAC", "IP Address", "Start Time", "Remaining (s)", "Status"});
    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setAlternatingRowColors(true);
}

void LeaseTableWidget::refresh(const QVector<LeaseInfo> &leases)
{
    setRowCount(leases.size());
    for (int i = 0; i < leases.size(); ++i) {
        const auto &lease = leases[i];

        setItem(i, 0, new QTableWidgetItem(lease.clientMac()));
        setItem(i, 1, new QTableWidgetItem(lease.ipAddress()));
        setItem(i, 2, new QTableWidgetItem(
            lease.startTime().toString("yyyy-MM-dd hh:mm:ss")));

        // remaining time with color gradient
        qint64 remaining = lease.remainingSeconds();
        auto *remainItem = new QTableWidgetItem(
            remaining > 0 ? QString::number(remaining) : "0 (Expired)");

        // red if < 60s, yellow if < 600s, green otherwise
        if (remaining <= 0)
            remainItem->setForeground(QColor("#f38ba8"));
        else if (remaining < 60)
            remainItem->setForeground(QColor("#f38ba8"));
        else if (remaining < 600)
            remainItem->setForeground(QColor("#f9e2af"));
        else
            remainItem->setForeground(QColor("#a6e3a1"));

        setItem(i, 3, remainItem);

        // status with color coding
        QString status;
        QColor statusColor;
        switch (lease.status()) {
        case LeaseStatus::Active:   status = "Active";   statusColor = "#a6e3a1"; break;
        case LeaseStatus::Expired:  status = "Expired";  statusColor = "#f38ba8"; break;
        case LeaseStatus::Released: status = "Released"; statusColor = "#6c7086"; break;
        case LeaseStatus::Renewed:  status = "Renewed";  statusColor = "#89b4fa"; break;
        }
        auto *statusItem = new QTableWidgetItem(status);
        statusItem->setForeground(statusColor);
        setItem(i, 4, statusItem);
    }
}
