#include "dhcprelayagent.h"
#include "service/dhcpserver.h"

DHCPRelayAgent::DHCPRelayAgent(DHCPServer *server, QObject *parent)
    : QObject(parent)
    , m_server(server)
    , m_relayIp(QString("%1.254").arg(m_clientSubnet))
{
}

void DHCPRelayAgent::forwardToServer(std::unique_ptr<DhcpMessage> msg)
{
    emit logMessage(QString("[Relay] Forward %1 to server (from subnet %2)")
                        .arg(msg->typeString(), m_clientSubnet));
    emit messageForwarded("Client→Server", msg->summary());
    // 实际转发逻辑：修改报文源地址为中继IP后发送给服务器
}

void DHCPRelayAgent::forwardToClient(std::unique_ptr<DhcpMessage> msg)
{
    emit logMessage(QString("[Relay] Forward %1 to client (target subnet %2)")
                        .arg(msg->typeString(), m_clientSubnet));
    emit messageForwarded("Server→Client", msg->summary());
    // 实际转发逻辑：将服务器响应转发给对应客户端
}
