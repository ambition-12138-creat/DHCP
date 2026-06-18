#ifndef LEASEINFO_H
#define LEASEINFO_H

#include <QString>
#include <QDateTime>

enum class LeaseStatus {
    Active,
    Expired,
    Released,
    Renewed
};

class LeaseInfo
{
public:
    LeaseInfo();

    QString clientMac() const { return m_clientMac; }
    void setClientMac(const QString &mac) { m_clientMac = mac; }

    QString ipAddress() const { return m_ipAddress; }
    void setIpAddress(const QString &ip) { m_ipAddress = ip; }

    QDateTime startTime() const { return m_startTime; }
    void setStartTime(const QDateTime &t) { m_startTime = t; }

    QDateTime endTime() const { return m_endTime; }
    void setEndTime(const QDateTime &t) { m_endTime = t; }

    int leaseDuration() const { return m_leaseDuration; }
    void setLeaseDuration(int seconds) { m_leaseDuration = seconds; }

    LeaseStatus status() const { return m_status; }
    void setStatus(LeaseStatus s) { m_status = s; }

    qint64 remainingSeconds() const;
    bool isExpired() const;
    void renew(int additionalSeconds);
    void release();

    QString summary() const;

private:
    QString m_clientMac;
    QString m_ipAddress;
    QDateTime m_startTime;
    QDateTime m_endTime;
    int m_leaseDuration = 86400; // 24h
    LeaseStatus m_status = LeaseStatus::Active;
};

#endif // LEASEINFO_H
