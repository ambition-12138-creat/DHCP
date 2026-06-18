#ifndef LEASETABLEWIDGET_H
#define LEASETABLEWIDGET_H

#include <QTableWidget>
#include <QVector>
#include "model/leaseinfo.h"

class LeaseTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    explicit LeaseTableWidget(QWidget *parent = nullptr);

    void refresh(const QVector<LeaseInfo> &leases);

signals:
    void leaseClicked(const QString &clientMac);

private:
    void setupTable();
};

#endif // LEASETABLEWIDGET_H
