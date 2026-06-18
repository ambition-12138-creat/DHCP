#include "mainwindow.h"
#include "controller/dhcpcontroller.h"
#include "controller/statemachine.h"
#include "service/ippoolmanager.h"
#include "service/leasemanager.h"
#include "view/iptablewidget.h"
#include "view/leasetablewidget.h"
#include "view/flowwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QInputDialog>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_controller(new DHCPController(this))
{
    setupUi();
    setupConnections();

    // 初始化地址池
    m_controller->init("192.168.1", 100, 150);

    // 预置3个客户端
    m_controller->addClient("AA:BB:CC:DD:00:01");
    m_controller->addClient("AA:BB:CC:DD:00:02");
    m_controller->addClient("AA:BB:CC:DD:00:03");

    // 更新客户端下拉框
    for (auto *client : m_controller->clients()) {
        m_clientCombo->addItem(QString("%1 (%2)")
            .arg(client->macAddress(), client->stateString()));
    }

    refreshAll();
    m_statusLabel->setText("就绪");
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi()
{
    auto *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto *mainLayout = new QVBoxLayout(centralWidget);

    // ======== 控制栏 ========
    auto *controlLayout = new QHBoxLayout();

    m_clientCombo = new QComboBox(this);
    m_clientCombo->setMinimumWidth(200);

    m_btnStart = new QPushButton("开始模拟", this);
    m_btnStartAll = new QPushButton("全部模拟", this);
    m_btnRenew = new QPushButton("续租", this);
    m_btnRelease = new QPushButton("释放", this);
    m_btnReset = new QPushButton("重置", this);
    m_btnAddClient = new QPushButton("添加客户端", this);
    m_btnRemoveClient = new QPushButton("移除客户端", this);

    controlLayout->addWidget(new QLabel("选择客户端:", this));
    controlLayout->addWidget(m_clientCombo);
    controlLayout->addWidget(m_btnStart);
    controlLayout->addWidget(m_btnStartAll);
    controlLayout->addWidget(m_btnRenew);
    controlLayout->addWidget(m_btnRelease);
    controlLayout->addWidget(m_btnReset);
    controlLayout->addWidget(m_btnAddClient);
    controlLayout->addWidget(m_btnRemoveClient);
    controlLayout->addStretch();

    auto *controlGroup = new QGroupBox("控制面板", this);
    controlGroup->setLayout(controlLayout);
    mainLayout->addWidget(controlGroup);

    // ======== 主内容区 (Splitter) ========
    auto *splitter = new QSplitter(Qt::Horizontal, this);

    // 左侧: 流程面板
    m_flowWidget = new FlowWidget(this);
    splitter->addWidget(m_flowWidget);

    // 中间: 表格面板
    auto *tableWidget = new QWidget(this);
    auto *tableLayout = new QVBoxLayout(tableWidget);
    tableLayout->setContentsMargins(0, 0, 0, 0);

    auto *ipGroup = new QGroupBox("IP 地址池", this);
    auto *ipLayout = new QVBoxLayout(ipGroup);
    m_ipTable = new IPTableWidget(this);
    ipLayout->addWidget(m_ipTable);
    tableLayout->addWidget(ipGroup);

    auto *leaseGroup = new QGroupBox("租约信息", this);
    auto *leaseLayout = new QVBoxLayout(leaseGroup);
    m_leaseTable = new LeaseTableWidget(this);
    leaseLayout->addWidget(m_leaseTable);
    tableLayout->addWidget(leaseGroup);

    splitter->addWidget(tableWidget);

    // 右侧: 日志面板
    auto *logGroup = new QGroupBox("系统日志", this);
    auto *logLayout = new QVBoxLayout(logGroup);
    m_logView = new QTextEdit(this);
    m_logView->setReadOnly(true);
    m_logView->setStyleSheet("font-family: Consolas, monospace; font-size: 12px;");
    m_logView->setMinimumWidth(300);
    logLayout->addWidget(m_logView);
    splitter->addWidget(logGroup);

    splitter->setStretchFactor(0, 1); // 流程
    splitter->setStretchFactor(1, 2); // 表格
    splitter->setStretchFactor(2, 2); // 日志

    mainLayout->addWidget(splitter, 1);

    // ======== 状态栏 ========
    m_statusLabel = new QLabel(this);
    statusBar()->addWidget(m_statusLabel);
}

void MainWindow::setupConnections()
{
    // 按钮 → 槽函数
    connect(m_btnStart, &QPushButton::clicked, this, &MainWindow::onStartSimulation);
    connect(m_btnStartAll, &QPushButton::clicked, this, &MainWindow::onStartAll);
    connect(m_btnRenew, &QPushButton::clicked, this, &MainWindow::onRenew);
    connect(m_btnRelease, &QPushButton::clicked, this, &MainWindow::onRelease);
    connect(m_btnReset, &QPushButton::clicked, this, &MainWindow::onReset);
    connect(m_btnAddClient, &QPushButton::clicked, this, &MainWindow::onAddClient);
    connect(m_btnRemoveClient, &QPushButton::clicked, this, &MainWindow::onRemoveClient);

    // 控制器 → UI 更新
    connect(m_controller, &DHCPController::logMessage,
            this, &MainWindow::onLogMessage);
    connect(m_controller, &DHCPController::simulationStep,
            this, &MainWindow::onSimulationStep);
    connect(m_controller, &DHCPController::clientStateChanged,
            this, &MainWindow::onClientStateChanged);
    connect(m_controller, &DHCPController::simulationComplete,
            this, &MainWindow::onSimulationComplete);

    // IP池变化 → 刷新表格
    connect(m_controller->poolManager(), &IPPoolManager::poolChanged,
            this, &MainWindow::refreshAll);
    // 租约变化 → 刷新表格
    connect(m_controller->leaseManager(), &LeaseManager::leasesChanged,
            this, &MainWindow::refreshAll);
}

void MainWindow::onStartSimulation()
{
    QString currentText = m_clientCombo->currentText();
    if (currentText.isEmpty()) return;

    QString mac = currentText.split(" ").first();
    m_controller->startSimulation(mac);
}

void MainWindow::onStartAll()
{
    m_controller->startAllSimulation();
}

void MainWindow::onRenew()
{
    QString currentText = m_clientCombo->currentText();
    if (currentText.isEmpty()) return;

    QString mac = currentText.split(" ").first();
    m_controller->renewLease(mac);
}

void MainWindow::onRelease()
{
    QString currentText = m_clientCombo->currentText();
    if (currentText.isEmpty()) return;

    QString mac = currentText.split(" ").first();
    m_controller->releaseLease(mac);
}

void MainWindow::onReset()
{
    m_controller->resetAll();
    m_flowWidget->clear();
}

void MainWindow::onAddClient()
{
    bool ok;
    QString mac = QInputDialog::getText(this, "添加客户端",
        "输入 MAC 地址 (如 AA:BB:CC:DD:00:04):",
        QLineEdit::Normal, "", &ok);

    if (ok && !mac.isEmpty()) {
        m_controller->addClient(mac);
        m_clientCombo->addItem(QString("%1 (Idle)").arg(mac));
    }
}

void MainWindow::onRemoveClient()
{
    QString currentText = m_clientCombo->currentText();
    if (currentText.isEmpty()) return;

    QString mac = currentText.split(" ").first();

    auto result = QMessageBox::question(this, "确认移除",
        QString("确定要移除客户端 %1 吗？\n其租约将被释放。").arg(mac));
    if (result == QMessageBox::Yes) {
        m_controller->removeClient(mac);
        m_clientCombo->removeItem(m_clientCombo->currentIndex());
    }
}

void MainWindow::onSimulationStep(const QString &step,
                                   std::shared_ptr<DhcpMessage> msg)
{
    m_flowWidget->addStep(step, msg);
    m_statusLabel->setText(QString("正在执行: %1").arg(step));
}

void MainWindow::onLogMessage(const QString &msg)
{
    m_logView->append(msg);
    // 自动滚动到底部
    QScrollBar *sb = m_logView->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void MainWindow::onClientStateChanged(const QString &mac, ClientState state)
{
    // 更新下拉框中的状态显示
    static const QStringList stateNames = {"Idle", "Selecting", "Requesting",
                                            "Bound", "Renewing", "Released"};
    for (int i = 0; i < m_clientCombo->count(); ++i) {
        if (m_clientCombo->itemText(i).startsWith(mac)) {
            m_clientCombo->setItemText(i, QString("%1 (%2)")
                .arg(mac, stateNames.value(static_cast<int>(state), "Unknown")));
            break;
        }
    }

    // 更新 MAC 地址的状态映射
    QString stateStr;
    switch (state) {
    case ClientState::Idle:       stateStr = "空闲"; break;
    case ClientState::Selecting:  stateStr = "选择中"; break;
    case ClientState::Requesting: stateStr = "请求中"; break;
    case ClientState::Bound:      stateStr = "已绑定"; break;
    case ClientState::Renewing:   stateStr = "续租中"; break;
    case ClientState::Released:   stateStr = "已释放"; break;
    }
    onLogMessage(QString("  → 客户端 %1 状态: %2").arg(mac, stateStr));
}

void MainWindow::onSimulationComplete(const QString &clientMac)
{
    m_statusLabel->setText(QString("模拟完成: %1").arg(clientMac));
}

void MainWindow::refreshAll()
{
    m_ipTable->refresh(m_controller->poolManager()->entries());
    m_leaseTable->refresh(m_controller->leaseManager()->activeLeases());
}
