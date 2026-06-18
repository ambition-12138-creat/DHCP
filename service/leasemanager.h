#ifndef LEASEMANAGER_H
#define LEASEMANAGER_H

#include <QObject>
#include <QVector>
#include <QTimer>
#include "model/leaseinfo.h"

class LeaseManager : public QObject
{
    Q_OBJECT

public:
    explicit LeaseManager(QObject *parent = nullptr);

    LeaseInfo *createLease(const QString &clientMac, const QString &ip,
                           int durationSeconds = 86400);
    bool renewLease(const QString &clientMac, int additionalSeconds = 86400);
    bool releaseLease(const QString &clientMac);
    bool releaseLeaseByIp(const QString &ip);

    LeaseInfo *findByClient(const QString &clientMac);
    LeaseInfo *findByIp(const QString &ip);

    QVector<LeaseInfo> activeLeases() const;
    void checkExpired(); // 检查并自动释放过期租约

    int activeCount() const;

signals:
    void leaseCreated(const QString &clientMac, const QString &ip);
    void leaseRenewed(const QString &clientMac, const QString &ip, qint64 remaining);
    void leaseExpired(const QString &clientMac, const QString &ip);
    void leaseReleased(const QString &clientMac, const QString &ip);
    void leasesChanged();

private:
    QVector<LeaseInfo> m_leases;
    QTimer *m_checkTimer;
};

#endif // LEASEMANAGER_H
