#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("DHCP Simulator");
    app.setApplicationVersion("1.0.0");

    MainWindow window;
    window.setWindowTitle("DHCP 协议模拟系统");
    window.resize(1024, 768);
    window.show();

    return app.exec();
}
