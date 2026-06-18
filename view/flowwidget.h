#ifndef FLOWWIDGET_H
#define FLOWWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>

class DhcpMessage;

class FlowWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FlowWidget(QWidget *parent = nullptr);

    void addStep(const QString &stepName, std::shared_ptr<DhcpMessage> msg);
    void clear();

signals:
    void stepClicked(const QString &stepName);

private:
    void setupUi();
    QListWidget *m_stepList;
    QLabel *m_titleLabel;
};

#endif // FLOWWIDGET_H
