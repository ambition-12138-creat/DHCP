#ifndef IPENTRY_H
#define IPENTRY_H

#include <QString>
#include <QDateTime>

class IPEntry
{
public:
    IPEntry();
    explicit IPEntry(const QString &ip);

    QString ipAddress() const { return m_ipAddress; }
    void setIpAddress(const QString &ip) { m_ipAddress = ip; }

    bool isAllocated() const { return m_allocated; }
    void setAllocated(bool allocated) { m_allocated = allocated; }

    QString clientMac() const { return m_clientMac; }
    void setClientMac(const QString &mac) { m_clientMac = mac; }

    QDateTime allocatedTime() const { return m_allocatedTime; }
    void setAllocatedTime(const QDateTime &t) { m_allocatedTime = t; }

    void allocate(const QString &mac);
    void release();
    QString statusString() const;

private:
    QString m_ipAddress;
    bool m_allocated = false;
    QString m_clientMac;
    QDateTime m_allocatedTime;
};

#endif // IPENTRY_H
