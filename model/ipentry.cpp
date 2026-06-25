#include "ipentry.h"

IPEntry::IPEntry()
{
}

IPEntry::IPEntry(const QString &ip)
    : m_ipAddress(ip)
{
}

void IPEntry::allocate(const QString &mac)
{
    m_allocated = true;
    m_clientMac = mac;
    m_allocatedTime = QDateTime::currentDateTime();
}

void IPEntry::release()
{
    m_allocated = false;
    m_clientMac.clear();
    m_allocatedTime = QDateTime();
}

QString IPEntry::statusString() const
{
    return m_allocated ? QString("Allocated (%1)").arg(m_clientMac) : "Available";
}
