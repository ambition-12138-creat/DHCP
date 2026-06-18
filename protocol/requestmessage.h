#ifndef REQUESTMESSAGE_H
#define REQUESTMESSAGE_H

#include "dhcpmessage.h"

class RequestMessage : public DhcpMessage
{
public:
    RequestMessage();

    QString requestedIp() const { return m_requestedIp; }
    void setRequestedIp(const QString &ip) { m_requestedIp = ip; }

    QString serverId() const { return m_serverId; }
    void setServerId(const QString &id) { m_serverId = id; }

    QString summary() const override;

private:
    QString m_requestedIp;
    QString m_serverId;
};

#endif // REQUESTMESSAGE_H
