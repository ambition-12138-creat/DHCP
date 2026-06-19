#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QToolBar>
#include <QDockWidget>
#include <QSplitter>
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
    void setupToolBar();
    void setupDockWidgets();
    void setupStatusBar();
    void refreshAll();
    void updateStats();

    // 控制器
    DHCPController *m_controller;

    // 视图组件
    FlowWidget *m_flowWidget;
    IPTableWidget *m_ipTable;
    LeaseTableWidget *m_leaseTable;
    QTextEdit *m_logView;

    // 工具栏
    QToolBar *m_toolBar;
    QComboBox *m_clientCombo;
    QPushButton *m_btnStart;
    QPushButton *m_btnStartAll;
    QPushButton *m_btnRenew;
    QPushButton *m_btnRelease;
    QPushButton *m_btnReset;
    QPushButton *m_btnAddClient;
    QPushButton *m_btnRemoveClient;

    // 停靠面板
    QDockWidget *m_flowDock;
    QDockWidget *m_logDock;

    // 状态栏
    QLabel *m_statusLabel;
    QLabel *m_poolStatsLabel;
    QLabel *m_leaseStatsLabel;
};

#endif // MAINWINDOW_H
