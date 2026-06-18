#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <QObject>
#include <QMap>
#include "model/dhcpclient.h"

class StateMachine : public QObject
{
    Q_OBJECT

public:
    explicit StateMachine(QObject *parent = nullptr);

    void registerClient(DHCPClient *client);
    void unregisterClient(const QString &mac);

    void transitionTo(const QString &mac, ClientState newState);
    ClientState currentState(const QString &mac) const;

    bool canTransition(ClientState from, ClientState to) const;

signals:
    void stateTransition(const QString &mac, ClientState from, ClientState to);
    void invalidTransition(const QString &mac, ClientState from, ClientState to);

private:
    void initTransitions();
    QMap<ClientState, QVector<ClientState>> m_transitions;
    QMap<QString, DHCPClient*> m_clients;
};

#endif // STATEMACHINE_H
