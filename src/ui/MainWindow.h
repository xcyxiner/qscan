#pragma once

#include <QMainWindow>

class ApplicationController;
class ControlPanel;
class LogPanel;
class ScanViewWidget;
class StatusPanel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(ApplicationController *controller, QWidget *parent = nullptr);

private:
    void bindState();

    ApplicationController *m_controller = nullptr;
    ScanViewWidget *m_scanViewWidget = nullptr;
    ControlPanel *m_controlPanel = nullptr;
    StatusPanel *m_statusPanel = nullptr;
    LogPanel *m_logPanel = nullptr;
};
