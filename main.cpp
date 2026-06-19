#include <QApplication>
#include <QPalette>
#include <QStyleFactory>
#include "mainwindow.h"

static void applyDarkTheme(QApplication &app)
{
    app.setStyle(QStyleFactory::create("Fusion"));

    QPalette p;
    p.setColor(QPalette::Window,          QColor("#1e1e2e"));
    p.setColor(QPalette::WindowText,      QColor("#cdd6f4"));
    p.setColor(QPalette::Base,            QColor("#181825"));
    p.setColor(QPalette::AlternateBase,   QColor("#1e1e2e"));
    p.setColor(QPalette::ToolTipBase,     QColor("#313244"));
    p.setColor(QPalette::ToolTipText,     QColor("#cdd6f4"));
    p.setColor(QPalette::Text,            QColor("#cdd6f4"));
    p.setColor(QPalette::Button,          QColor("#313244"));
    p.setColor(QPalette::ButtonText,      QColor("#cdd6f4"));
    p.setColor(QPalette::BrightText,      QColor("#f38ba8"));
    p.setColor(QPalette::Link,            QColor("#89b4fa"));
    p.setColor(QPalette::Highlight,       QColor("#89b4fa"));
    p.setColor(QPalette::HighlightedText, QColor("#1e1e2e"));

    p.setColor(QPalette::Disabled, QPalette::WindowText, QColor("#6c7086"));
    p.setColor(QPalette::Disabled, QPalette::Text,       QColor("#6c7086"));
    p.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#6c7086"));

    app.setPalette(p);

    app.setStyleSheet(R"(
        /* ── Global ── */
        * { font-size: 13px; }

        /* ── MainWindow ── */
        QMainWindow::separator {
            width: 2px; height: 2px;
            background: #45475a;
        }

        /* ── ToolBar ── */
        QToolBar {
            background: #181825;
            border-bottom: 2px solid #313244;
            spacing: 6px;
            padding: 4px 8px;
        }
        QToolBar QPushButton {
            background: #313244;
            border: 1px solid #45475a;
            border-radius: 5px;
            padding: 5px 14px;
            color: #cdd6f4;
            font-weight: bold;
        }
        QToolBar QPushButton:hover {
            background: #45475a;
            border-color: #89b4fa;
        }
        QToolBar QPushButton:pressed {
            background: #585b70;
        }
        QToolBar QPushButton:disabled {
            color: #585b70;
            background: #1e1e2e;
        }
        QToolBar QComboBox {
            background: #313244;
            border: 1px solid #45475a;
            border-radius: 5px;
            padding: 4px 10px;
            color: #cdd6f4;
            min-width: 200px;
        }
        QToolBar QComboBox:hover { border-color: #89b4fa; }
        QToolBar QComboBox::drop-down {
            border: none;
            width: 20px;
        }
        QToolBar QComboBox QAbstractItemView {
            background: #313244;
            color: #cdd6f4;
            selection-background-color: #45475a;
            border: 1px solid #45475a;
        }

        /* ── Dock Widget ── */
        QDockWidget {
            color: #cdd6f4;
            titlebar-close-icon: none;
            titlebar-normal-icon: none;
        }
        QDockWidget::title {
            background: #181825;
            padding: 6px 10px;
            border-bottom: 1px solid #313244;
            text-align: left;
        }

        /* ── GroupBox ── */
        QGroupBox {
            font-weight: bold;
            border: 1px solid #45475a;
            border-radius: 6px;
            margin-top: 12px;
            padding-top: 16px;
            color: #cdd6f4;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            padding: 2px 10px;
            color: #89b4fa;
        }

        /* ── PushButton (general) ── */
        QPushButton {
            background: #313244;
            border: 1px solid #45475a;
            border-radius: 5px;
            padding: 6px 16px;
            color: #cdd6f4;
        }
        QPushButton:hover { background: #45475a; border-color: #89b4fa; }
        QPushButton:pressed { background: #585b70; }

        /* ── ComboBox ── */
        QComboBox {
            background: #313244;
            border: 1px solid #45475a;
            border-radius: 5px;
            padding: 4px 10px;
            color: #cdd6f4;
        }
        QComboBox::drop-down { border: none; width: 20px; }
        QComboBox QAbstractItemView {
            background: #313244;
            color: #cdd6f4;
            selection-background-color: #45475a;
            border: 1px solid #45475a;
        }

        /* ── TableWidget ── */
        QTableWidget {
            background: #181825;
            alternate-background-color: #1e1e2e;
            gridline-color: #313244;
            border: 1px solid #45475a;
            border-radius: 4px;
            selection-background-color: #45475a;
            color: #cdd6f4;
        }
        QHeaderView::section {
            background: #313244;
            color: #89b4fa;
            font-weight: bold;
            padding: 6px 8px;
            border: none;
            border-right: 1px solid #45475a;
            border-bottom: 2px solid #89b4fa;
        }

        /* ── ListWidget ── */
        QListWidget {
            background: #181825;
            alternate-background-color: #1e1e2e;
            border: 1px solid #45475a;
            border-radius: 4px;
            color: #cdd6f4;
            outline: none;
        }
        QListWidget::item { padding: 4px 8px; border-left: 3px solid transparent; }
        QListWidget::item:selected { background: #313244; }

        /* ── TextEdit (Log) ── */
        QTextEdit {
            background: #11111b;
            color: #a6e3a1;
            font-family: "Consolas", "Courier New", monospace;
            font-size: 12px;
            border: 1px solid #45475a;
            border-radius: 4px;
            selection-background-color: #45475a;
        }

        /* ── ScrollBar ── */
        QScrollBar:vertical {
            background: #181825;
            width: 10px;
            margin: 0;
            border-radius: 5px;
        }
        QScrollBar::handle:vertical {
            background: #45475a;
            min-height: 30px;
            border-radius: 5px;
        }
        QScrollBar::handle:vertical:hover { background: #585b70; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0; border: none;
        }
        QScrollBar:horizontal {
            background: #181825;
            height: 10px;
            margin: 0;
            border-radius: 5px;
        }
        QScrollBar::handle:horizontal {
            background: #45475a;
            min-width: 30px;
            border-radius: 5px;
        }
        QScrollBar::handle:horizontal:hover { background: #585b70; }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0; border: none;
        }

        /* ── StatusBar ── */
        QStatusBar {
            background: #181825;
            border-top: 2px solid #313244;
            color: #a6adc8;
            padding: 2px 8px;
        }
        QStatusBar::item { border: none; }

        /* ── ToolTip ── */
        QToolTip {
            background: #313244;
            color: #cdd6f4;
            border: 1px solid #45475a;
            border-radius: 4px;
            padding: 4px;
        }

        /* ── Splitter ── */
        QSplitter::handle {
            background: #45475a;
        }

        /* ── Menu ── */
        QMenuBar {
            background: #181825;
            color: #cdd6f4;
            border-bottom: 1px solid #313244;
        }
        QMenuBar::item:selected { background: #313244; }
        QMenu {
            background: #313244;
            color: #cdd6f4;
            border: 1px solid #45475a;
        }
        QMenu::item:selected { background: #45475a; }
    )");
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("DhcpSimulator");
    app.setOrganizationName("DhcpSim");
    app.setApplicationVersion("1.0.0");

    applyDarkTheme(app);

    MainWindow window;
    window.setWindowTitle("DHCP 协议模拟系统");
    window.resize(1200, 800);
    window.setMinimumSize(900, 600);
    window.show();

    return app.exec();
}
