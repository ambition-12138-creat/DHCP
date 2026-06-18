#include "ippoolmanager.h"
#include <algorithm>

IPPoolManager::IPPoolManager(QObject *parent)
    : QObject(parent)
{
}

void IPPoolManager::initPool(const QString &baseNetwork, int startHost, int endHost)
{
    m_pool.clear();
    m_baseNetwork = baseNetwork;
    for (int i = startHost; i <= endHost; ++i) {
        IPEntry entry(QString("%1.%2").arg(baseNetwork).arg(i));
        m_pool.append(entry);
    }
    emit poolChanged();
}

void IPPoolManager::initPool(const QStringList &ipList)
{
    m_pool.clear();
    for (const auto &ip : ipList) {
        m_pool.append(IPEntry(ip));
    }
    emit poolChanged();
}

QString IPPoolManager::allocateIp(const QString &clientMac)
{
    for (auto &entry : m_pool) {
        if (!entry.isAllocated()) {
            entry.allocate(clientMac);
            emit ipAllocated(entry.ipAddress(), clientMac);
            emit poolChanged();
            return entry.ipAddress();
        }
    }
    return QString(); // 无可用地址
}

bool IPPoolManager::releaseIp(const QString &ip)
{
    auto *entry = findByIp(ip);
    if (entry && entry->isAllocated()) {
        entry->release();
        emit ipReleased(ip);
        emit poolChanged();
        return true;
    }
    return false;
}

bool IPPoolManager::releaseIpByClient(const QString &clientMac)
{
    auto *entry = findByClient(clientMac);
    if (entry && entry->isAllocated()) {
        entry->release();
        emit ipReleased(entry->ipAddress());
        emit poolChanged();
        return true;
    }
    return false;
}

bool IPPoolManager::isAvailable(const QString &ip) const
{
    auto it = std::find_if(m_pool.begin(), m_pool.end(),
        [&ip](const IPEntry &e) { return e.ipAddress() == ip; });
    return it != m_pool.end() && !it->isAllocated();
}

int IPPoolManager::availableCount() const
{
    return std::count_if(m_pool.begin(), m_pool.end(),
        [](const IPEntry &e) { return !e.isAllocated(); });
}

int IPPoolManager::totalCount() const
{
    return m_pool.size();
}

QStringList IPPoolManager::availableIps() const
{
    QStringList list;
    for (const auto &entry : m_pool) {
        if (!entry.isAllocated())
            list.append(entry.ipAddress());
    }
    return list;
}

IPEntry *IPPoolManager::findByIp(const QString &ip)
{
    auto it = std::find_if(m_pool.begin(), m_pool.end(),
        [&ip](const IPEntry &e) { return e.ipAddress() == ip; });
    return it != m_pool.end() ? &(*it) : nullptr;
}

IPEntry *IPPoolManager::findByClient(const QString &clientMac)
{
    auto it = std::find_if(m_pool.begin(), m_pool.end(),
        [&clientMac](const IPEntry &e) { return e.clientMac() == clientMac; });
    return it != m_pool.end() ? &(*it) : nullptr;
}
