#ifndef DHCPMESSAGE_H
#define DHCPMESSAGE_H

#include <QString>
#include <QDateTime>

enum class DhcpMessageType {
    Discover,
    Offer,
    Request,
    Ack,
    Renew
};

class DhcpMessage
{
public:
    DhcpMessage(DhcpMessageType type);
    virtual ~DhcpMessage() = default;

    DhcpMessageType type() const { return m_type; }
    QString typeString() const;

    // 公共字段
    QString transactionId() const { return m_transactionId; }
    void setTransactionId(const QString &id) { m_transactionId = id; }

    QString clientMac() const { return m_clientMac; }
    void setClientMac(const QString &mac) { m_clientMac = mac; }

    QDateTime timestamp() const { return m_timestamp; }
    void setTimestamp(const QDateTime &ts) { m_timestamp = ts; }

    // 报文摘要（供日志显示）
    virtual QString summary() const;

protected:
    DhcpMessageType m_type;
    QString m_transactionId;
    QString m_clientMac;
    QDateTime m_timestamp;
};

#endif // DHCPMESSAGE_H
