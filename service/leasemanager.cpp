#include "leasemanager.h"
#include <algorithm>

LeaseManager::LeaseManager(QObject *parent)
    : QObject(parent)
    , m_checkTimer(new QTimer(this))
{
    connect(m_checkTimer, &QTimer::timeout, this, &LeaseManager::checkExpired);
    m_checkTimer->start(1000); // 每秒检查
}

LeaseInfo *LeaseManager::createLease(const QString &clientMac, const QString &ip,
                                      int durationSeconds)
{
    LeaseInfo lease;
    lease.setClientMac(clientMac);
    lease.setIpAddress(ip);
    lease.setLeaseDuration(durationSeconds);
    lease.setStartTime(QDateTime::currentDateTime());
    lease.setEndTime(QDateTime::currentDateTime().addSecs(durationSeconds));
    lease.setStatus(LeaseStatus::Active);

    m_leases.append(lease);
    emit leaseCreated(clientMac, ip);
    emit leasesChanged();
    return &m_leases.last();
}

bool LeaseManager::renewLease(const QString &clientMac, int additionalSeconds)
{
    auto *lease = findByClient(clientMac);
    if (!lease)
        return false;
    if (lease->status() != LeaseStatus::Active &&
        lease->status() != LeaseStatus::Renewed)
        return false;

    lease->renew(additionalSeconds);
    emit leaseRenewed(clientMac, lease->ipAddress(), lease->remainingSeconds());
    emit leasesChanged();
    return true;
}

bool LeaseManager::releaseLease(const QString &clientMac)
{
    auto *lease = findByClient(clientMac);
    if (!lease)
        return false;

    lease->release();
    emit leaseReleased(clientMac, lease->ipAddress());
    emit leasesChanged();
    return true;
}

bool LeaseManager::releaseLeaseByIp(const QString &ip)
{
    auto *lease = findByIp(ip);
    if (!lease)
        return false;

    emit leaseReleased(lease->clientMac(), ip);
    lease->release();
    emit leasesChanged();
    return true;
}

LeaseInfo *LeaseManager::findByClient(const QString &clientMac)
{
    auto it = std::find_if(m_leases.begin(), m_leases.end(),
        [&clientMac](const LeaseInfo &l) { return l.clientMac() == clientMac; });
    return it != m_leases.end() ? &(*it) : nullptr;
}

LeaseInfo *LeaseManager::findByIp(const QString &ip)
{
    auto it = std::find_if(m_leases.begin(), m_leases.end(),
        [&ip](const LeaseInfo &l) { return l.ipAddress() == ip; });
    return it != m_leases.end() ? &(*it) : nullptr;
}

QVector<LeaseInfo> LeaseManager::activeLeases() const
{
    QVector<LeaseInfo> active;
    for (const auto &l : m_leases) {
        if ((l.status() == LeaseStatus::Active ||
             l.status() == LeaseStatus::Renewed) && !l.isExpired())
            active.append(l);
    }
    return active;
}

void LeaseManager::checkExpired()
{
    for (auto &lease : m_leases) {
        if (lease.status() == LeaseStatus::Active && lease.isExpired()) {
            lease.setStatus(LeaseStatus::Expired);
            emit leaseExpired(lease.clientMac(), lease.ipAddress());
            emit leasesChanged();
        }
    }
}

int LeaseManager::activeCount() const
{
    return activeLeases().size();
}
