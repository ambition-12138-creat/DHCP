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
    case ClientState::Idle:       return "Idle";
    case ClientState::Selecting:  return "Selecting";
    case ClientState::Requesting: return "Requesting";
    case ClientState::Bound:      return "Bound";
    case ClientState::Renewing:   return "Renewing";
    case ClientState::Released:   return "Released";
    }
    return "Unknown";
}
