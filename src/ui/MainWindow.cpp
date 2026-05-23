#include "ui/MainWindow.h"

#include "app/ApplicationController.h"
#include "ui/ControlPanel.h"
#include "ui/LogPanel.h"
#include "ui/ScanViewWidget.h"
#include "ui/StatusPanel.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(ApplicationController *controller, QWidget *parent)
    : QMainWindow(parent)
    , m_controller(controller)
    , m_scanViewWidget(new ScanViewWidget(this))
    , m_controlPanel(new ControlPanel(this))
    , m_statusPanel(new StatusPanel(this))
    , m_logPanel(new LogPanel(this))
{
    auto *centralWidget = new QWidget(this);
    auto *rootLayout = new QHBoxLayout(centralWidget);
    auto *sidebarLayout = new QVBoxLayout();

    sidebarLayout->addWidget(m_controlPanel);
    sidebarLayout->addWidget(m_statusPanel);
    sidebarLayout->addWidget(m_logPanel, 1);

    rootLayout->addWidget(m_scanViewWidget, 3);
    rootLayout->addLayout(sidebarLayout, 1);

    setCentralWidget(centralWidget);
    setWindowTitle(QStringLiteral("QScan 演示原型"));
    resize(1280, 720);

    connect(m_controlPanel, &ControlPanel::settingsChanged, m_controller, &ApplicationController::updateProcessingSettings);
    connect(m_controlPanel, &ControlPanel::startRequested, m_controller, &ApplicationController::startStreaming);
    connect(m_controlPanel, &ControlPanel::stopRequested, m_controller, &ApplicationController::stopStreaming);

    bindState();
}

void MainWindow::bindState()
{
    AppState *state = m_controller->appState();

    m_controlPanel->setSettings(state->processingSettings());
    m_statusPanel->setConnectionState(state->connectionState());
    m_statusPanel->setStreamState(state->streamState());
    m_statusPanel->setFrameNumber(state->currentFrameNumber());
    m_scanViewWidget->setImage(state->currentImage());

    for (const LogEntry &entry : state->logEntries()) {
        m_logPanel->appendEntry(entry);
    }

    connect(state, &AppState::processingSettingsChanged, m_controlPanel, &ControlPanel::setSettings);
    connect(state, &AppState::connectionStateChanged, m_statusPanel, &StatusPanel::setConnectionState);
    connect(state, &AppState::streamStateChanged, this, [this, state](AppState::StreamState streamState) {
        m_statusPanel->setStreamState(streamState);
        if (streamState == AppState::StreamState::Idle && state->currentImage().isNull()) {
            m_scanViewWidget->setPlaceholderText(QStringLiteral("等待扫描数据"));
        }
        if (streamState == AppState::StreamState::Error) {
            m_scanViewWidget->setPlaceholderText(QStringLiteral("扫描链路出现异常"));
        }
    });
    connect(state, &AppState::currentImageChanged, this, [this](const QImage &image, quint64 frameNumber) {
        m_scanViewWidget->setImage(image);
        m_statusPanel->setFrameNumber(frameNumber);
    });
    connect(state, &AppState::logAdded, m_logPanel, &LogPanel::appendEntry);
}
