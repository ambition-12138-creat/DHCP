#ifndef RENEWMESSAGE_H
#define RENEWMESSAGE_H

#include "dhcpmessage.h"

class RenewMessage : public DhcpMessage
{
public:
    RenewMessage()
        : DhcpMessage(DhcpMessageType::Renew)
    {
    }

    QString currentIp() const { return m_currentIp; }
    void setCurrentIp(const QString &ip) { m_currentIp = ip; }

    QString serverId() const { return m_serverId; }
    void setServerId(const QString &id) { m_serverId = id; }

    QString summary() const override
    {
        return QString("[Renew] MAC=%1 CurrentIP=%2 Server=%3")
            .arg(clientMac(), m_currentIp, m_serverId);
    }

private:
    QString m_currentIp;
    QString m_serverId;
};

#endif // RENEWMESSAGE_H
