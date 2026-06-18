#ifndef OFFERMESSAGE_H
#define OFFERMESSAGE_H

#include "dhcpmessage.h"

class OfferMessage : public DhcpMessage
{
public:
    OfferMessage()
        : DhcpMessage(DhcpMessageType::Offer)
    {
    }

    QString offeredIp() const { return m_offeredIp; }
    void setOfferedIp(const QString &ip) { m_offeredIp = ip; }

    QString serverId() const { return m_serverId; }
    void setServerId(const QString &id) { m_serverId = id; }

    int leaseTime() const { return m_leaseTime; }
    void setLeaseTime(int seconds) { m_leaseTime = seconds; }

    QString subnetMask() const { return m_subnetMask; }
    void setSubnetMask(const QString &mask) { m_subnetMask = mask; }

    QString router() const { return m_router; }
    void setRouter(const QString &r) { m_router = r; }

    QString dnsServer() const { return m_dnsServer; }
    void setDnsServer(const QString &dns) { m_dnsServer = dns; }

    QString summary() const override
    {
        return QString("[Offer] Server=%1 OfferedIP=%2 LeaseTime=%3s → MAC=%4")
            .arg(m_serverId, m_offeredIp)
            .arg(m_leaseTime)
            .arg(clientMac());
    }

private:
    QString m_offeredIp;
    QString m_serverId;
    int m_leaseTime = 86400;
    QString m_subnetMask = "255.255.255.0";
    QString m_router;
    QString m_dnsServer;
};

#endif // OFFERMESSAGE_H
