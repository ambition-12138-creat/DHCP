#include "statemachine.h"

StateMachine::StateMachine(QObject *parent)
    : QObject(parent)
{
    initTransitions();
}

void StateMachine::initTransitions()
{
    // 定义合法状态转换
    m_transitions[ClientState::Idle]       = {ClientState::Selecting};
    m_transitions[ClientState::Selecting]  = {ClientState::Requesting, ClientState::Idle};
    m_transitions[ClientState::Requesting] = {ClientState::Bound, ClientState::Idle};
    m_transitions[ClientState::Bound]      = {ClientState::Renewing, ClientState::Released};
    m_transitions[ClientState::Renewing]   = {ClientState::Bound, ClientState::Released};
    m_transitions[ClientState::Released]   = {ClientState::Idle};
}

void StateMachine::registerClient(DHCPClient *client)
{
    m_clients.insert(client->macAddress(), client);
}

void StateMachine::unregisterClient(const QString &mac)
{
    m_clients.remove(mac);
}

void StateMachine::transitionTo(const QString &mac, ClientState newState)
{
    auto *client = m_clients.value(mac, nullptr);
    if (!client)
        return;

    ClientState oldState = client->state();
    if (!canTransition(oldState, newState)) {
        emit invalidTransition(mac, oldState, newState);
        return;
    }

    client->setState(newState);
    emit stateTransition(mac, oldState, newState);
}

ClientState StateMachine::currentState(const QString &mac) const
{
    auto *client = m_clients.value(mac, nullptr);
    return client ? client->state() : ClientState::Idle;
}

bool StateMachine::canTransition(ClientState from, ClientState to) const
{
    return m_transitions.value(from).contains(to);
}
