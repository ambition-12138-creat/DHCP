#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QSplitter>
#include <QLabel>
#include <memory>
#include "protocol/dhcpmessage.h"
#include "model/dhcpclient.h"

class DHCPController;
class IPTableWidget;
class LeaseTableWidget;
class FlowWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onStartSimulation();
    void onStartAll();
    void onRenew();
    void onRelease();
    void onReset();
    void onAddClient();
    void onRemoveClient();

    void onSimulationStep(const QString &step, std::shared_ptr<DhcpMessage> msg);
    void onLogMessage(const QString &msg);
    void onClientStateChanged(const QString &mac, ClientState state);
    void onSimulationComplete(const QString &clientMac);

private:
    void setupUi();
    void setupConnections();
    void refreshAll();

    // 控制器
    DHCPController *m_controller;

    // 视图组件
    IPTableWidget *m_ipTable;
    LeaseTableWidget *m_leaseTable;
    FlowWidget *m_flowWidget;
    QTextEdit *m_logView;

    // 控制区
    QPushButton *m_btnStart;
    QPushButton *m_btnStartAll;
    QPushButton *m_btnRenew;
    QPushButton *m_btnRelease;
    QPushButton *m_btnReset;
    QPushButton *m_btnAddClient;
    QPushButton *m_btnRemoveClient;
    QComboBox *m_clientCombo;
    QLabel *m_statusLabel;
};

#endif // MAINWINDOW_H
