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
    setHorizontalHeaderLabels({"客户端 MAC", "IP 地址", "开始时间", "剩余时间(s)", "状态"});
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
            remaining > 0 ? QString::number(remaining) : "0 (已过期)");

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
        case LeaseStatus::Active:   status = "活跃";   statusColor = "#a6e3a1"; break;
        case LeaseStatus::Expired:  status = "已过期"; statusColor = "#f38ba8"; break;
        case LeaseStatus::Released: status = "已释放"; statusColor = "#6c7086"; break;
        case LeaseStatus::Renewed:  status = "已续租"; statusColor = "#89b4fa"; break;
        }
        auto *statusItem = new QTableWidgetItem(status);
        statusItem->setForeground(statusColor);
        setItem(i, 4, statusItem);
    }
}
