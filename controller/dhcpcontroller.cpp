#include "dhcpcontroller.h"
#include "controller/statemachine.h"
#include "service/ippoolmanager.h"
#include "service/leasemanager.h"
#include "service/dhcpserver.h"
#include "service/dhcprelayagent.h"
#include "protocol/discovermessage.h"
#include "protocol/offermessage.h"
#include "protocol/requestmessage.h"
#include "protocol/ackmessage.h"
#include "protocol/renewmessage.h"

DHCPController::DHCPController(QObject *parent)
    : QObject(parent)
    , m_stepTimer(new QTimer(this))
{
    m_poolManager = new IPPoolManager(this);
    m_leaseManager = new LeaseManager(this);
    m_server = new DHCPServer(m_poolManager, m_leaseManager, this);
    m_relayAgent = new DHCPRelayAgent(m_server, this);
    m_stateMachine = new StateMachine(this);

    m_stepTimer->setSingleShot(true);

    // 转发服务器的日志
    connect(m_server, &DHCPServer::logMessage,
            this, &DHCPController::logMessage);
    connect(m_relayAgent, &DHCPRelayAgent::logMessage,
            this, &DHCPController::logMessage);
}

DHCPController::~DHCPController() = default;

void DHCPController::init(const QString &baseNetwork, int startHost, int endHost)
{
    m_poolManager->initPool(baseNetwork, startHost, endHost);
    emit logMessage(QString("[系统] IP地址池初始化: %1.%2 - %1.%3")
                        .arg(baseNetwork).arg(startHost).arg(endHost));
}

DHCPClient *DHCPController::addClient(const QString &mac)
{
    auto *client = new DHCPClient(mac, this);
    m_clients.append(client);
    m_stateMachine->registerClient(client);

    connect(client, &DHCPClient::stateChanged,
            this, &DHCPController::clientStateChanged);

    emit logMessage(QString("[系统] 客户端已添加: MAC=%1").arg(mac));
    return client;
}

void DHCPController::removeClient(const QString &mac)
{
    for (int i = 0; i < m_clients.size(); ++i) {
        if (m_clients[i]->macAddress() == mac) {
            releaseLease(mac);
            m_stateMachine->unregisterClient(mac);
            m_clients.removeAt(i);
            emit logMessage(QString("[系统] 客户端已移除: MAC=%1").arg(mac));
            return;
        }
    }
}

void DHCPController::startSimulation(const QString &clientMac)
{
    auto *client = std::find_if(m_clients.begin(), m_clients.end(),
        [&](DHCPClient *c) { return c->macAddress() == clientMac; });
    if (client == m_clients.end()) {
        emit logMessage(QString("[错误] 客户端不存在: %1").arg(clientMac));
        return;
    }

    emit logMessage(QString("========== 开始 DHCP 四步交互 (%1) ==========").arg(clientMac));

    // Step 1: Discover
    m_stateMachine->transitionTo(clientMac, ClientState::Selecting);
    doDiscover(clientMac);
}

void DHCPController::startAllSimulation()
{
    for (auto *client : m_clients) {
        startSimulation(client->macAddress());
    }
}

void DHCPController::doDiscover(const QString &clientMac)
{
    DiscoverMessage discover;
    discover.setClientMac(clientMac);
    discover.setTransactionId(QString("TX-%1-%2")
        .arg(clientMac.right(4))
        .arg(QDateTime::currentMSecsSinceEpoch()));

    emit simulationStep("Discover", std::make_shared<DiscoverMessage>(discover));
    emit logMessage(discover.summary());

    // 延迟后执行 Step 2
    connect(m_stepTimer, &QTimer::timeout, this, [this, clientMac]() {
        // 服务器处理 Discover → 返回 Offer
        DiscoverMessage discover;
        discover.setClientMac(clientMac);
        auto offer = m_server->processDiscover(discover);

        if (offer) {
            m_pendingOffers[clientMac] = {offer->offeredIp(), offer->serverId()};
            emit simulationStep("Offer", std::make_shared<OfferMessage>(*offer));
            emit logMessage(offer->summary());

            // Step 3: 客户端发起 Request
            QTimer::singleShot(500, this, [this, clientMac]() {
                doRequest(clientMac,
                          m_pendingOffers[clientMac].offeredIp,
                          m_pendingOffers[clientMac].serverId);
            });
        }
    }, Qt::SingleShotConnection);
    m_stepTimer->start(500);
}

void DHCPController::doRequest(const QString &clientMac,
                                const QString &offeredIp,
                                const QString &serverId)
{
    m_stateMachine->transitionTo(clientMac, ClientState::Requesting);

    RequestMessage request;
    request.setClientMac(clientMac);
    request.setRequestedIp(offeredIp);
    request.setServerId(serverId);
    request.setTransactionId(QString("TX-%1-%2")
        .arg(clientMac.right(4))
        .arg(QDateTime::currentMSecsSinceEpoch()));

    emit simulationStep("Request", std::make_shared<RequestMessage>(request));
    emit logMessage(request.summary());

    // 延迟后执行 Step 4
    QTimer::singleShot(500, this, [this, clientMac, offeredIp, serverId]() {
        // 服务器处理 Request → 返回 ACK
        RequestMessage req;
        req.setClientMac(clientMac);
        req.setRequestedIp(offeredIp);
        req.setServerId(serverId);
        auto ack = m_server->processRequest(req);

        if (ack) {
            m_stateMachine->transitionTo(clientMac, ClientState::Bound);

            // 更新客户端IP
            auto it = std::find_if(m_clients.begin(), m_clients.end(),
                [&](DHCPClient *c) { return c->macAddress() == clientMac; });
            if (it != m_clients.end())
                (*it)->setAssignedIp(offeredIp);

            emit simulationStep("ACK", std::make_shared<AckMessage>(*ack));
            emit logMessage(ack->summary());
        }

        m_pendingOffers.remove(clientMac);
        emit simulationComplete(clientMac);
        emit logMessage(QString("========== DHCP 交互完成 (%1) ==========").arg(clientMac));
    });
}

void DHCPController::renewLease(const QString &clientMac)
{
    m_stateMachine->transitionTo(clientMac, ClientState::Renewing);
    doRenew(clientMac);
}

void DHCPController::doRenew(const QString &clientMac)
{
    auto *client = std::find_if(m_clients.begin(), m_clients.end(),
        [&](DHCPClient *c) { return c->macAddress() == clientMac; });

    RenewMessage renew;
    renew.setClientMac(clientMac);
    renew.setCurrentIp(client != m_clients.end() ? (*client)->assignedIp() : "");
    renew.setServerId(m_server->serverId());
    renew.setTransactionId(QString("TX-RENEW-%1-%2")
        .arg(clientMac.right(4))
        .arg(QDateTime::currentMSecsSinceEpoch()));

    emit simulationStep("Renew", std::make_shared<RenewMessage>(renew));
    emit logMessage(renew.summary());

    auto ack = m_server->processRenew(renew);
    if (ack) {
        m_stateMachine->transitionTo(clientMac, ClientState::Bound);
        emit simulationStep("ACK", std::make_shared<AckMessage>(*ack));
    }
}

void DHCPController::releaseLease(const QString &clientMac)
{
    m_poolManager->releaseIpByClient(clientMac);
    m_leaseManager->releaseLease(clientMac);
    m_stateMachine->transitionTo(clientMac, ClientState::Released);

    auto it = std::find_if(m_clients.begin(), m_clients.end(),
        [&](DHCPClient *c) { return c->macAddress() == clientMac; });
    if (it != m_clients.end())
        (*it)->setAssignedIp(QString());

    emit logMessage(QString("[系统] 租约已释放: MAC=%1").arg(clientMac));
}

void DHCPController::resetAll()
{
    for (auto *client : m_clients) {
        releaseLease(client->macAddress());
        m_stateMachine->transitionTo(client->macAddress(), ClientState::Idle);
    }
    m_pendingOffers.clear();
    emit logMessage("[系统] 系统已重置");
}
