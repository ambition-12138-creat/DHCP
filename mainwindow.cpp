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
#include <QStatusBar>
#include <QSettings>
#include <QMenuBar>

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
        m_clientCombo->addItem(QString("%1  [%2]")
            .arg(client->macAddress(), client->stateString()));
    }

    refreshAll();
    m_statusLabel->setText("就绪");

    // 恢复停靠面板布局
    QSettings settings;
    restoreState(settings.value("windowState").toByteArray());
    restoreGeometry(settings.value("windowGeometry").toByteArray());
}

MainWindow::~MainWindow()
{
    QSettings settings;
    settings.setValue("windowState", saveState());
    settings.setValue("windowGeometry", saveGeometry());
}

// ─── UI Setup ───────────────────────────────────────────

void MainWindow::setupUi()
{
    setupToolBar();
    setupDockWidgets();
    setupStatusBar();
    setCorner(Qt::BottomLeftCorner,  Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    setDockOptions(AnimatedDocks | AllowNestedDocks | AllowTabbedDocks);

    QMenuBar *mb = menuBar();
    QMenu *viewMenu = mb->addMenu("视图(&V)");
    viewMenu->addAction(m_flowDock->toggleViewAction());
    viewMenu->addAction(m_logDock->toggleViewAction());
}

void MainWindow::setupToolBar()
{
    m_toolBar = addToolBar("主工具栏");
    m_toolBar->setObjectName("mainToolBar");
    m_toolBar->setMovable(false);
    m_toolBar->setFloatable(false);

    // 客户端选择器
    auto *lbl = new QLabel("客户端 ", this);
    lbl->setStyleSheet("color: #89b4fa; font-weight: bold; background: transparent;");
    m_toolBar->addWidget(lbl);

    m_clientCombo = new QComboBox(this);
    m_clientCombo->setMinimumWidth(220);
    m_clientCombo->setToolTip("选择要操作的客户端");
    m_toolBar->addWidget(m_clientCombo);

    m_toolBar->addSeparator();

    // 操作按钮
    m_btnStart    = new QPushButton("▶ 开始模拟", this);
    m_btnStartAll = new QPushButton("▶▶ 全部模拟", this);
    m_btnRenew    = new QPushButton("↻ 续租", this);
    m_btnRelease  = new QPushButton("✕ 释放", this);
    m_btnReset    = new QPushButton("⟲ 重置", this);

    m_btnStart   ->setToolTip("对选中的客户端执行 DHCP 四步交互");
    m_btnStartAll->setToolTip("对所有客户端依次执行 DHCP 交互");
    m_btnRenew   ->setToolTip("续租选中客户端的 IP 租约");
    m_btnRelease ->setToolTip("释放选中客户端的 IP 租约");
    m_btnReset   ->setToolTip("重置整个系统状态");

    m_toolBar->addWidget(m_btnStart);
    m_toolBar->addWidget(m_btnStartAll);
    m_toolBar->addWidget(m_btnRenew);
    m_toolBar->addWidget(m_btnRelease);
    m_toolBar->addWidget(m_btnReset);

    m_toolBar->addSeparator();

    // 客户端管理按钮
    m_btnAddClient    = new QPushButton("＋ 添加", this);
    m_btnRemoveClient = new QPushButton("－ 移除", this);

    m_btnAddClient   ->setToolTip("添加一个新的客户端");
    m_btnRemoveClient->setToolTip("移除选中的客户端");

    m_toolBar->addWidget(m_btnAddClient);
    m_toolBar->addWidget(m_btnRemoveClient);

    // 右侧弹簧 + 统计
    auto *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_toolBar->addWidget(spacer);

    m_poolStatsLabel = new QLabel(this);
    m_poolStatsLabel->setStyleSheet("color: #a6e3a1; font-weight: bold; background: transparent; padding: 0 8px;");
    m_toolBar->addWidget(m_poolStatsLabel);

    m_leaseStatsLabel = new QLabel(this);
    m_leaseStatsLabel->setStyleSheet("color: #89b4fa; font-weight: bold; background: transparent; padding: 0 8px;");
    m_toolBar->addWidget(m_leaseStatsLabel);
}

void MainWindow::setupDockWidgets()
{
    // ── 左侧：交互流程 ──
    m_flowWidget = new FlowWidget(this);
    m_flowDock = new QDockWidget("DHCP 交互流程", this);
    m_flowDock->setObjectName("flowDock");
    m_flowDock->setWidget(m_flowWidget);
    m_flowDock->setMinimumWidth(280);
    m_flowDock->setFeatures(QDockWidget::DockWidgetMovable |
                            QDockWidget::DockWidgetFloatable |
                            QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::LeftDockWidgetArea, m_flowDock);

    // ── 右侧：系统日志 ──
    m_logView = new QTextEdit(this);
    m_logView->setReadOnly(true);
    m_logView->setMinimumWidth(320);

    m_logDock = new QDockWidget("系统日志", this);
    m_logDock->setObjectName("logDock");
    m_logDock->setWidget(m_logView);
    m_logDock->setFeatures(QDockWidget::DockWidgetMovable |
                           QDockWidget::DockWidgetFloatable |
                           QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::RightDockWidgetArea, m_logDock);

    // ── 中央：表格区域（垂直分割） ──
    auto *centerSplitter = new QSplitter(Qt::Vertical, this);

    // IP 地址池
    auto *ipGroup = new QGroupBox("IP 地址池", this);
    auto *ipLayout = new QVBoxLayout(ipGroup);
    m_ipTable = new IPTableWidget(this);
    ipLayout->addWidget(m_ipTable);
    centerSplitter->addWidget(ipGroup);

    // 租约信息
    auto *leaseGroup = new QGroupBox("租约信息", this);
    auto *leaseLayout = new QVBoxLayout(leaseGroup);
    m_leaseTable = new LeaseTableWidget(this);
    leaseLayout->addWidget(m_leaseTable);
    centerSplitter->addWidget(leaseGroup);

    centerSplitter->setStretchFactor(0, 1);
    centerSplitter->setStretchFactor(1, 1);

    setCentralWidget(centerSplitter);
}

void MainWindow::setupStatusBar()
{
    m_statusLabel = new QLabel(this);
    m_statusLabel->setStyleSheet("color: #a6adc8; padding-left: 4px;");
    statusBar()->addWidget(m_statusLabel, 1);
}

// ─── Connections ────────────────────────────────────────

void MainWindow::setupConnections()
{
    // 按钮 → 槽函数
    connect(m_btnStart,    &QPushButton::clicked, this, &MainWindow::onStartSimulation);
    connect(m_btnStartAll, &QPushButton::clicked, this, &MainWindow::onStartAll);
    connect(m_btnRenew,    &QPushButton::clicked, this, &MainWindow::onRenew);
    connect(m_btnRelease,  &QPushButton::clicked, this, &MainWindow::onRelease);
    connect(m_btnReset,    &QPushButton::clicked, this, &MainWindow::onReset);
    connect(m_btnAddClient,    &QPushButton::clicked, this, &MainWindow::onAddClient);
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

// ─── Slots ──────────────────────────────────────────────

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
        m_clientCombo->addItem(QString("%1  [Idle]").arg(mac));
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
    QScrollBar *sb = m_logView->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void MainWindow::onClientStateChanged(const QString &mac, ClientState state)
{
    static const QStringList stateNames = {"空闲", "选择中", "请求中",
                                            "已绑定", "续租中", "已释放"};
    for (int i = 0; i < m_clientCombo->count(); ++i) {
        if (m_clientCombo->itemText(i).startsWith(mac)) {
            m_clientCombo->setItemText(i, QString("%1  [%2]")
                .arg(mac, stateNames.value(static_cast<int>(state), "Unknown")));
            break;
        }
    }

    onLogMessage(QString("  → 客户端 %1 状态: %2")
        .arg(mac, stateNames.value(static_cast<int>(state), "未知")));
}

void MainWindow::onSimulationComplete(const QString &clientMac)
{
    m_statusLabel->setText(QString("模拟完成: %1").arg(clientMac));
}

// ─── Refresh ────────────────────────────────────────────

void MainWindow::refreshAll()
{
    m_ipTable->refresh(m_controller->poolManager()->entries());
    m_leaseTable->refresh(m_controller->leaseManager()->activeLeases());
    updateStats();
}

void MainWindow::updateStats()
{
    auto *pm = m_controller->poolManager();
    auto *lm = m_controller->leaseManager();

    int total = pm->totalCount();
    int avail = pm->availableCount();
    int used  = total - avail;
    int pct   = total > 0 ? (used * 100 / total) : 0;

    m_poolStatsLabel->setText(
        QString("IP池: %1/%2 (%3%)").arg(used).arg(total).arg(pct));
    m_leaseStatsLabel->setText(
        QString("活跃租约: %1").arg(lm->activeCount()));
}
