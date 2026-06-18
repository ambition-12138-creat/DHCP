#include "leaseinfo.h"
#include <QDateTime>

LeaseInfo::LeaseInfo()
    : m_startTime(QDateTime::currentDateTime())
    , m_endTime(m_startTime.addSecs(m_leaseDuration))
{
}

qint64 LeaseInfo::remainingSeconds() const
{
    return QDateTime::currentDateTime().secsTo(m_endTime);
}

bool LeaseInfo::isExpired() const
{
    return remainingSeconds() <= 0;
}

void LeaseInfo::renew(int additionalSeconds)
{
    m_endTime = m_endTime.addSecs(additionalSeconds);
    m_status = LeaseStatus::Renewed;
}

void LeaseInfo::release()
{
    m_endTime = QDateTime::currentDateTime();
    m_status = LeaseStatus::Released;
}

QString LeaseInfo::summary() const
{
    return QString("MAC=%1 IP=%2 Remaining=%3s Status=%4")
        .arg(m_clientMac, m_ipAddress)
        .arg(remainingSeconds())
        .arg(m_status == LeaseStatus::Active ? "Active" : "Expired");
}
