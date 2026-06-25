#include "dhcpserver.h"
#include "service/ippoolmanager.h"
#include "service/leasemanager.h"

DHCPServer::DHCPServer(IPPoolManager *pool, LeaseManager *leases,
                         QObject *parent)
    : QObject(parent)
    , m_pool(pool)
    , m_leases(leases)
{
}

std::unique_ptr<OfferMessage> DHCPServer::processDiscover(const DiscoverMessage &msg)
{
    emit logMessage(QString("[Server] Received Discover (MAC=%1)").arg(msg.clientMac()));

    // 从地址池分配IP
    QString offeredIp = m_pool->allocateIp(msg.clientMac());
    if (offeredIp.isEmpty()) {
        emit logMessage("[Server] No available IP address!");
        return nullptr;
    }

    auto offer = std::make_unique<OfferMessage>();
    offer->setTransactionId(msg.transactionId());
    offer->setClientMac(msg.clientMac());
    offer->setOfferedIp(offeredIp);
    offer->setServerId(m_serverId);
    offer->setLeaseTime(msg.leaseTimeRequested());
    offer->setSubnetMask(m_subnetMask);
    offer->setRouter(m_router);
    offer->setDnsServer(m_dnsServer);

    emit offerSent(msg.clientMac(), offeredIp);
    emit logMessage(QString("[Server] Sent Offer (IP=%1 -> MAC=%2)")
                        .arg(offeredIp, msg.clientMac()));
    return offer;
}

std::unique_ptr<AckMessage> DHCPServer::processRequest(const RequestMessage &msg)
{
    emit logMessage(QString("[Server] Received Request (MAC=%1, IP=%2)")
                        .arg(msg.clientMac(), msg.requestedIp()));

    auto ack = std::make_unique<AckMessage>();
    ack->setTransactionId(msg.transactionId());
    ack->setClientMac(msg.clientMac());
    ack->setAssignedIp(msg.requestedIp());
    ack->setServerId(m_serverId);
    ack->setLeaseTime(86400);
    ack->setSubnetMask(m_subnetMask);
    ack->setRouter(m_router);
    ack->setDnsServer(m_dnsServer);

    // 创建租约记录
    m_leases->createLease(msg.clientMac(), msg.requestedIp(), 86400);

    emit ackSent(msg.clientMac(), msg.requestedIp());
    emit logMessage(QString("[Server] Sent ACK (IP=%1 -> MAC=%2)")
                        .arg(msg.requestedIp(), msg.clientMac()));
    return ack;
}

std::unique_ptr<AckMessage> DHCPServer::processRenew(const RenewMessage &msg)
{
    emit logMessage(QString("[Server] Received Renew (MAC=%1, IP=%2)")
                        .arg(msg.clientMac(), msg.currentIp()));

    if (m_leases->renewLease(msg.clientMac())) {
        auto ack = std::make_unique<AckMessage>();
        ack->setTransactionId(msg.transactionId());
        ack->setClientMac(msg.clientMac());
        ack->setAssignedIp(msg.currentIp());
        ack->setServerId(m_serverId);
        ack->setLeaseTime(86400);

        emit ackSent(msg.clientMac(), msg.currentIp());
        emit logMessage(QString("[Server] Renew success (IP=%1 -> MAC=%2)")
                            .arg(msg.currentIp(), msg.clientMac()));
        return ack;
    }

    emit logMessage(QString("[Server] Renew failed (MAC=%1)").arg(msg.clientMac()));
    return nullptr;
}
