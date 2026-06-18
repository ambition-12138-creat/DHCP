#ifndef IPPOOLMANAGER_H
#define IPPOOLMANAGER_H

#include <QObject>
#include <QVector>
#include "model/ipentry.h"

class IPPoolManager : public QObject
{
    Q_OBJECT

public:
    explicit IPPoolManager(QObject *parent = nullptr);

    void initPool(const QString &baseNetwork, int startHost, int endHost);
    void initPool(const QStringList &ipList);

    QString allocateIp(const QString &clientMac);
    bool releaseIp(const QString &ip);
    bool releaseIpByClient(const QString &clientMac);

    bool isAvailable(const QString &ip) const;
    int availableCount() const;
    int totalCount() const;

    QVector<IPEntry> entries() const { return m_pool; }
    QStringList availableIps() const;
    IPEntry *findByIp(const QString &ip);
    IPEntry *findByClient(const QString &clientMac);

signals:
    void poolChanged();
    void ipAllocated(const QString &ip, const QString &clientMac);
    void ipReleased(const QString &ip);

private:
    QVector<IPEntry> m_pool;
    QString m_baseNetwork; // e.g., "192.168.1"
};

#endif // IPPOOLMANAGER_H
