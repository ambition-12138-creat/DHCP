#include "dhcpclient.h"

DHCPClient::DHCPClient(const QString &mac, QObject *parent)
    : QObject(parent)
    , m_macAddress(mac)
    , m_clientId(QString("client-%1").arg(mac.right(8)))
{
}

void DHCPClient::setState(ClientState state)
{
    if (m_state != state) {
        m_state = state;
        emit stateChanged(m_macAddress, m_state);

        if (state == ClientState::Released) {
            emit ipReleased(m_macAddress);
        }
    }
}

QString DHCPClient::stateString() const
{
    switch (m_state) {
    case ClientState::Idle:       return "空闲";
    case ClientState::Selecting:  return "选择中";
    case ClientState::Requesting: return "请求中";
    case ClientState::Bound:      return "已绑定";
    case ClientState::Renewing:   return "续租中";
    case ClientState::Released:   return "已释放";
    }
    return "未知";
}
