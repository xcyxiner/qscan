#pragma once

#include "core/ProcessingSettings.h"

#include <QDateTime>
#include <QImage>
#include <QObject>
#include <QString>
#include <QVector>

struct LogEntry
{
    QDateTime timestamp;
    QString level;
    QString message;

    [[nodiscard]] bool operator==(const LogEntry &other) const
    {
        return timestamp == other.timestamp
            && level == other.level
            && message == other.message;
    }
};

class AppState : public QObject
{
    Q_OBJECT

public:
    enum class ConnectionState
    {
        Disconnected,
        Connecting,
        Connected
    };
    Q_ENUM(ConnectionState)

    enum class StreamState
    {
        Idle,
        Starting,
        Streaming,
        Stopping,
        Error
    };
    Q_ENUM(StreamState)

    explicit AppState(QObject *parent = nullptr);

    [[nodiscard]] const ProcessingSettings &processingSettings() const;
    [[nodiscard]] ConnectionState connectionState() const;
    [[nodiscard]] StreamState streamState() const;
    [[nodiscard]] const QImage &currentImage() const;
    [[nodiscard]] quint64 currentFrameNumber() const;
    [[nodiscard]] const QVector<LogEntry> &logEntries() const;

    bool setProcessingSettings(const ProcessingSettings &settings);
    bool setConnectionState(ConnectionState state);
    bool setStreamState(StreamState state);
    bool setCurrentImage(const QImage &image, quint64 frameNumber);
    bool appendLog(const QString &level, const QString &message);

signals:
    void processingSettingsChanged(const ProcessingSettings &settings);
    void connectionStateChanged(ConnectionState state);
    void streamStateChanged(StreamState state);
    void currentImageChanged(const QImage &image, quint64 frameNumber);
    void logAdded(const LogEntry &entry);

private:
    ProcessingSettings m_processingSettings;
    ConnectionState m_connectionState = ConnectionState::Disconnected;
    StreamState m_streamState = StreamState::Idle;
    QImage m_currentImage;
    quint64 m_currentFrameNumber = 0;
    QVector<LogEntry> m_logEntries;
};

Q_DECLARE_METATYPE(LogEntry)
Q_DECLARE_METATYPE(AppState::ConnectionState)
Q_DECLARE_METATYPE(AppState::StreamState)
