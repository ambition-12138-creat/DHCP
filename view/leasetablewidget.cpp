#include "leasetablewidget.h"
#include <QHeaderView>

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
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
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
        setItem(i, 2, new QTableWidgetItem(lease.startTime().toString("yyyy-MM-dd hh:mm:ss")));
        setItem(i, 3, new QTableWidgetItem(QString::number(lease.remainingSeconds())));
        QString status;
        switch (lease.status()) {
        case LeaseStatus::Active:   status = "活跃"; break;
        case LeaseStatus::Expired:  status = "已过期"; break;
        case LeaseStatus::Released: status = "已释放"; break;
        case LeaseStatus::Renewed:  status = "已续租"; break;
        }
        setItem(i, 4, new QTableWidgetItem(status));
    }
}
