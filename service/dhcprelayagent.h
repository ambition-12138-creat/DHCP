#ifndef DHCPRELAYAGENT_H
#define DHCPRELAYAGENT_H

#include <QObject>
#include "protocol/dhcpmessage.h"

class DHCPServer;

class DHCPRelayAgent : public QObject
{
    Q_OBJECT

public:
    explicit DHCPRelayAgent(DHCPServer *server, QObject *parent = nullptr);

    void setRelayIp(const QString &ip) { m_relayIp = ip; }
    QString relayIp() const { return m_relayIp; }

    void setClientSubnet(const QString &subnet) { m_clientSubnet = subnet; }
    QString clientSubnet() const { return m_clientSubnet; }

    // 转发报文：客户端 → 服务器
    void forwardToServer(std::unique_ptr<DhcpMessage> msg);
    // 转发报文：服务器 → 客户端
    void forwardToClient(std::unique_ptr<DhcpMessage> msg);

signals:
    void messageForwarded(const QString &direction, const QString &summary);
    void logMessage(const QString &msg);

private:
    DHCPServer *m_server;
    QString m_relayIp;
    QString m_clientSubnet = "10.0.0"; // 模拟不同网段
};

#endif // DHCPRELAYAGENT_H
