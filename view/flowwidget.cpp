#include "flowwidget.h"
#include "protocol/dhcpmessage.h"

FlowWidget::FlowWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void FlowWidget::setupUi()
{
    auto *layout = new QVBoxLayout(this);

    m_titleLabel = new QLabel("DHCP 交互流程", this);
    m_titleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    layout->addWidget(m_titleLabel);

    m_stepList = new QListWidget(this);
    m_stepList->setAlternatingRowColors(true);
    layout->addWidget(m_stepList);

    setMinimumWidth(250);
}

void FlowWidget::addStep(const QString &stepName, std::shared_ptr<DhcpMessage> msg)
{
    // 为每个步骤设置对应图标颜色
    QString color;
    if (stepName == "Discover") color = "#2196F3";
    else if (stepName == "Offer") color = "#4CAF50";
    else if (stepName == "Request") color = "#FF9800";
    else if (stepName == "ACK") color = "#9C27B0";
    else if (stepName == "Renew") color = "#00BCD4";
    else color = "#757575";

    QString text = QString("[%1] %2 %3")
        .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
        .arg(stepName)
        .arg(msg ? msg->clientMac() : "");

    auto *item = new QListWidgetItem(text);
    item->setForeground(QColor(color));
    m_stepList->addItem(item);
    m_stepList->scrollToBottom();
}

void FlowWidget::clear()
{
    m_stepList->clear();
}
