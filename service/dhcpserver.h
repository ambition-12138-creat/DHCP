#ifndef DHCPSERVER_H
#define DHCPSERVER_H

#include <QObject>
#include <memory>
#include "protocol/dhcpmessage.h"
#include "protocol/discovermessage.h"
#include "protocol/offermessage.h"
#include "protocol/requestmessage.h"
#include "protocol/ackmessage.h"
#include "protocol/renewmessage.h"

class IPPoolManager;
class LeaseManager;

class DHCPServer : public QObject
{
    Q_OBJECT

public:
    explicit DHCPServer(IPPoolManager *pool, LeaseManager *leases,
                         QObject *parent = nullptr);

    QString serverId() const { return m_serverId; }
    void setServerId(const QString &id) { m_serverId = id; }

    QString subnetMask() const { return m_subnetMask; }
    void setSubnetMask(const QString &mask) { m_subnetMask = mask; }

    QString router() const { return m_router; }
    void setRouter(const QString &r) { m_router = r; }

    QString dnsServer() const { return m_dnsServer; }
    void setDnsServer(const QString &dns) { m_dnsServer = dns; }

    std::unique_ptr<OfferMessage> processDiscover(const DiscoverMessage &msg);
    std::unique_ptr<AckMessage> processRequest(const RequestMessage &msg);
    std::unique_ptr<AckMessage> processRenew(const RenewMessage &msg);

signals:
    void offerSent(const QString &clientMac, const QString &offeredIp);
    void ackSent(const QString &clientMac, const QString &assignedIp);
    void logMessage(const QString &msg);

private:
    IPPoolManager *m_pool;
    LeaseManager *m_leases;
    QString m_serverId = "192.168.1.1";
    QString m_subnetMask = "255.255.255.0";
    QString m_router = "192.168.1.1";
    QString m_dnsServer = "8.8.8.8";
};

#endif // DHCPSERVER_H
