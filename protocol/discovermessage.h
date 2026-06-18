#ifndef DISCOVERMESSAGE_H
#define DISCOVERMESSAGE_H

#include "dhcpmessage.h"

class DiscoverMessage : public DhcpMessage
{
public:
    DiscoverMessage();

    int leaseTimeRequested() const { return m_leaseTimeRequested; }
    void setLeaseTimeRequested(int seconds) { m_leaseTimeRequested = seconds; }

    QString requestedIp() const { return m_requestedIp; }
    void setRequestedIp(const QString &ip) { m_requestedIp = ip; }

    QString summary() const override;

private:
    int m_leaseTimeRequested = 86400; // 默认24h
    QString m_requestedIp;             // 客户端可选指定IP
};

#endif // DISCOVERMESSAGE_H
