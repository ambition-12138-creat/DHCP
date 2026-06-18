#ifndef DISCOVERMESSAGE_H
#define DISCOVERMESSAGE_H

#include "dhcpmessage.h"

class DiscoverMessage : public DhcpMessage
{
public:
    DiscoverMessage()
        : DhcpMessage(DhcpMessageType::Discover)
    {
    }

    int leaseTimeRequested() const { return m_leaseTimeRequested; }
    void setLeaseTimeRequested(int seconds) { m_leaseTimeRequested = seconds; }

    QString requestedIp() const { return m_requestedIp; }
    void setRequestedIp(const QString &ip) { m_requestedIp = ip; }

    QString summary() const override
    {
        return QString("[Discover] MAC=%1 TransactionID=%2 RequestedIP=%3")
            .arg(clientMac(), transactionId(),
                 m_requestedIp.isEmpty() ? "any" : m_requestedIp);
    }

private:
    int m_leaseTimeRequested = 86400;
    QString m_requestedIp;
};

#endif // DISCOVERMESSAGE_H
