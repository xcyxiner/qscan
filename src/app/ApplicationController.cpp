#include "app/ApplicationController.h"

#include "pipeline/ScanPipeline.h"

ApplicationController::ApplicationController(QObject *parent)
    : QObject(parent)
    , m_appState(this)
    , m_scanPipeline(new ScanPipeline(this))
{
    connect(m_scanPipeline, &ScanPipeline::frameReady, this, [this](const QImage &image, quint64 frameNumber) {
        m_appState.setConnectionState(AppState::ConnectionState::Connected);
        m_appState.setStreamState(AppState::StreamState::Streaming);
        publishProcessedImage(image, frameNumber);
    });
    connect(m_scanPipeline, &ScanPipeline::connectionStateChanged, &m_appState, &AppState::setConnectionState);
    connect(m_scanPipeline, &ScanPipeline::streamStateChanged, &m_appState, &AppState::setStreamState);
    connect(m_scanPipeline, &ScanPipeline::errorOccurred, this, &ApplicationController::setStreamError);

    appendLog(QStringLiteral("info"), QStringLiteral("QScan controller initialized"));
}

AppState *ApplicationController::appState()
{
    return &m_appState;
}

const AppState *ApplicationController::appState() const
{
    return &m_appState;
}

ProcessingSettings ApplicationController::processingSettings() const
{
    return m_appState.processingSettings();
}

bool ApplicationController::updateProcessingSettings(const ProcessingSettings &settings)
{
    const bool changed = m_appState.setProcessingSettings(settings);
    if (!changed) {
        return false;
    }

    m_scanPipeline->updateSettings(settings);
    appendLog(QStringLiteral("info"), QStringLiteral("Processing settings updated"));
    emit processingSettingsRequested(settings);
    return true;
}

bool ApplicationController::startStreaming()
{
    if (m_appState.streamState() == AppState::StreamState::Streaming || m_appState.streamState() == AppState::StreamState::Starting) {
        return false;
    }

    m_appState.setConnectionState(AppState::ConnectionState::Connecting);
    m_appState.setStreamState(AppState::StreamState::Starting);
    appendLog(QStringLiteral("info"), QStringLiteral("Starting simulated scanner"));
    m_scanPipeline->start();
    emit streamingStarted();

    return true;
}

bool ApplicationController::stopStreaming()
{
    if (m_appState.streamState() == AppState::StreamState::Idle && m_appState.connectionState() == AppState::ConnectionState::Disconnected) {
        return false;
    }

    m_appState.setStreamState(AppState::StreamState::Stopping);
    appendLog(QStringLiteral("info"), QStringLiteral("Stopping simulated scanner"));
    m_scanPipeline->stop();
    emit streamingStopped();

    return true;
}

bool ApplicationController::publishProcessedImage(const QImage &image, quint64 frameNumber)
{
    return m_appState.setCurrentImage(image, frameNumber);
}

void ApplicationController::appendLog(const QString &level, const QString &message)
{
    m_appState.appendLog(level, message);
}

void ApplicationController::setStreamError(const QString &message)
{
    m_appState.setStreamState(AppState::StreamState::Error);
    m_appState.setConnectionState(AppState::ConnectionState::Disconnected);
    appendLog(QStringLiteral("error"), message);
}

