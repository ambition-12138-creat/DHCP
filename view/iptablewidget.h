#ifndef IPTABLEWIDGET_H
#define IPTABLEWIDGET_H

#include <QTableWidget>
#include <QVector>
#include "model/ipentry.h"

class IPTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    explicit IPTableWidget(QWidget *parent = nullptr);

    void refresh(const QVector<IPEntry> &entries);
    void updateEntry(const IPEntry &entry);

signals:
    void ipClicked(const QString &ip);

private:
    void setupTable();
};

#endif // IPTABLEWIDGET_H
