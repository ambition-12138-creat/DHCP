#include "flowwidget.h"
#include "protocol/dhcpmessage.h"
#include <QDateTime>

FlowWidget::FlowWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void FlowWidget::setupUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_titleLabel = new QLabel("DHCP Flow", this);
    m_titleLabel->setStyleSheet(
        "font-weight: bold; font-size: 14px; color: #89b4fa; padding: 4px 0;");
    layout->addWidget(m_titleLabel);

    m_stepList = new QListWidget(this);
    m_stepList->setAlternatingRowColors(true);
    m_stepList->setStyleSheet(
        "QListWidget { background: #11111b; border: 1px solid #313244; border-radius: 4px; }"
        "QListWidget::item { padding: 6px 10px; border-left: 4px solid transparent; }"
        "QListWidget::item:selected { background: #313244; }");
    layout->addWidget(m_stepList);
}

void FlowWidget::addStep(const QString &stepName, std::shared_ptr<DhcpMessage> msg)
{
    QString color;
    QString icon;
    if (stepName == "Discover")      { color = "#89b4fa"; icon = "●"; }
    else if (stepName == "Offer")    { color = "#a6e3a1"; icon = "◆"; }
    else if (stepName == "Request")  { color = "#fab387"; icon = "▲"; }
    else if (stepName == "ACK")      { color = "#cba6f7"; icon = "★"; }
    else if (stepName == "Renew")    { color = "#94e2d5"; icon = "↻"; }
    else                             { color = "#a6adc8"; icon = "○"; }

    QString ts = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString mac = msg ? msg->clientMac() : "";

    auto *item = new QListWidgetItem(
        QString("<span style='color:#585b70;'>%1</span>  "
                "<span style='color:%2;'>%3</span>  "
                "<b style='color:%4;'>%5</b>"
                "<span style='color:#6c7086; margin-left:8px;'>%6</span>")
            .arg(ts, color, icon, color, stepName, mac));
    m_stepList->addItem(item);
    m_stepList->scrollToBottom();
}

void FlowWidget::clear()
{
    m_stepList->clear();
}
