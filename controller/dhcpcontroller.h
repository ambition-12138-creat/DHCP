#ifndef DHCPCONTROLLER_H
#define DHCPCONTROLLER_H

#include <QObject>
#include <QTimer>
#include <memory>
#include "model/dhcpclient.h"
#include "protocol/dhcpmessage.h"

class IPPoolManager;
class LeaseManager;
class DHCPServer;
class DHCPRelayAgent;
class StateMachine;
class DiscoverMessage;
class OfferMessage;
class RequestMessage;
class AckMessage;
class RenewMessage;

class DHCPController : public QObject
{
    Q_OBJECT

public:
    explicit DHCPController(QObject *parent = nullptr);
    ~DHCPController() override;

    // 初始化
    void init(const QString &baseNetwork, int startHost, int endHost);

    // 客户端管理
    DHCPClient *addClient(const QString &mac);
    void removeClient(const QString &mac);
    QList<DHCPClient*> clients() const { return m_clients; }

    // 模拟控制
    void startSimulation(const QString &clientMac);
    void startAllSimulation();
    void renewLease(const QString &clientMac);
    void releaseLease(const QString &clientMac);
    void resetAll();

    // 访问子模块
    IPPoolManager *poolManager() const { return m_poolManager; }
    LeaseManager *leaseManager() const { return m_leaseManager; }
    DHCPServer *server() const { return m_server; }
    DHCPRelayAgent *relayAgent() const { return m_relayAgent; }
    StateMachine *stateMachine() const { return m_stateMachine; }

signals:
    void logMessage(const QString &msg);
    void simulationStep(const QString &step, std::shared_ptr<DhcpMessage> msg);
    void simulationComplete(const QString &clientMac);
    void clientStateChanged(const QString &mac, ClientState state);

private slots:
    void doDiscover(const QString &clientMac);
    void doRequest(const QString &clientMac, const QString &offeredIp,
                   const QString &serverId);
    void doRenew(const QString &clientMac);

private:
    IPPoolManager *m_poolManager;
    LeaseManager *m_leaseManager;
    DHCPServer *m_server;
    DHCPRelayAgent *m_relayAgent;
    StateMachine *m_stateMachine;

    QList<DHCPClient*> m_clients;
    QTimer *m_stepTimer; // 用于模拟步骤间延迟

    // 暂存当前 Offer 信息
    struct PendingOffer {
        QString offeredIp;
        QString serverId;
    };
    QMap<QString, PendingOffer> m_pendingOffers;
};

#endif // DHCPCONTROLLER_H
