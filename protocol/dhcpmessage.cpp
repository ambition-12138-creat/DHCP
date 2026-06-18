#include "dhcpmessage.h"

DhcpMessage::DhcpMessage(DhcpMessageType type)
    : m_type(type)
    , m_timestamp(QDateTime::currentDateTime())
{
}

QString DhcpMessage::typeString() const
{
    switch (m_type) {
    case DhcpMessageType::Discover: return "Discover";
    case DhcpMessageType::Offer:    return "Offer";
    case DhcpMessageType::Request:  return "Request";
    case DhcpMessageType::Ack:      return "ACK";
    case DhcpMessageType::Renew:    return "Renew";
    }
    return "Unknown";
}

QString DhcpMessage::summary() const
{
    return QString("[%1] MAC=%2 TransactionID=%3")
        .arg(typeString(), m_clientMac, m_transactionId);
}
