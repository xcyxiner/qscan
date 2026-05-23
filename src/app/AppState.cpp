#include "app/AppState.h"

AppState::AppState(QObject *parent)
    : QObject(parent)
{
}

const ProcessingSettings &AppState::processingSettings() const
{
    return m_processingSettings;
}

AppState::ConnectionState AppState::connectionState() const
{
    return m_connectionState;
}

AppState::StreamState AppState::streamState() const
{
    return m_streamState;
}

const QImage &AppState::currentImage() const
{
    return m_currentImage;
}

quint64 AppState::currentFrameNumber() const
{
    return m_currentFrameNumber;
}

const QVector<LogEntry> &AppState::logEntries() const
{
    return m_logEntries;
}

bool AppState::setProcessingSettings(const ProcessingSettings &settings)
{
    if (m_processingSettings == settings) {
        return false;
    }

    m_processingSettings = settings;
    emit processingSettingsChanged(m_processingSettings);
    return true;
}

bool AppState::setConnectionState(ConnectionState state)
{
    if (m_connectionState == state) {
        return false;
    }

    m_connectionState = state;
    emit connectionStateChanged(m_connectionState);
    return true;
}

bool AppState::setStreamState(StreamState state)
{
    if (m_streamState == state) {
        return false;
    }

    m_streamState = state;
    emit streamStateChanged(m_streamState);
    return true;
}

bool AppState::setCurrentImage(const QImage &image, quint64 frameNumber)
{
    if (m_currentFrameNumber == frameNumber && m_currentImage.cacheKey() == image.cacheKey()) {
        return false;
    }

    m_currentImage = image;
    m_currentFrameNumber = frameNumber;
    emit currentImageChanged(m_currentImage, m_currentFrameNumber);
    return true;
}

bool AppState::appendLog(const QString &level, const QString &message)
{
    LogEntry entry;
    entry.timestamp = QDateTime::currentDateTimeUtc();
    entry.level = level;
    entry.message = message;

    m_logEntries.append(entry);
    emit logAdded(entry);
    return true;
}
