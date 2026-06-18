#ifndef ACKMESSAGE_H
#define ACKMESSAGE_H

#include "dhcpmessage.h"

class AckMessage : public DhcpMessage
{
public:
    AckMessage();

    QString assignedIp() const { return m_assignedIp; }
    void setAssignedIp(const QString &ip) { m_assignedIp = ip; }

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

    QString summary() const override;

private:
    QString m_assignedIp;
    QString m_serverId;
    int m_leaseTime = 86400;
    QString m_subnetMask = "255.255.255.0";
    QString m_router;
    QString m_dnsServer;
};

#endif // ACKMESSAGE_H
