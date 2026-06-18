#ifndef DHCPCLIENT_H
#define DHCPCLIENT_H

#include <QString>
#include <QObject>

enum class ClientState {
    Idle,          // 初始空闲
    Selecting,     // 已发送Discover，等待Offer
    Requesting,    // 已发送Request，等待ACK
    Bound,         // 已绑定IP
    Renewing,      // 续租中
    Released       // 已释放
};

class DHCPClient : public QObject
{
    Q_OBJECT

public:
    explicit DHCPClient(const QString &mac, QObject *parent = nullptr);

    QString macAddress() const { return m_macAddress; }
    QString clientId() const { return m_clientId; }

    ClientState state() const { return m_state; }
    void setState(ClientState state);

    QString assignedIp() const { return m_assignedIp; }
    void setAssignedIp(const QString &ip) { m_assignedIp = ip; }

    QString stateString() const;

signals:
    void stateChanged(const QString &mac, ClientState newState);
    void ipAssigned(const QString &mac, const QString &ip);
    void ipReleased(const QString &mac);

private:
    QString m_macAddress;
    QString m_clientId;
    ClientState m_state = ClientState::Idle;
    QString m_assignedIp;
};

#endif // DHCPCLIENT_H
