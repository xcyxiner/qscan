#pragma once

#include "app/AppState.h"

#include <QObject>

class ScanPipeline;

class ApplicationController : public QObject
{
    Q_OBJECT

public:
    explicit ApplicationController(QObject *parent = nullptr);

    [[nodiscard]] AppState *appState();
    [[nodiscard]] const AppState *appState() const;
    [[nodiscard]] ProcessingSettings processingSettings() const;

    bool updateProcessingSettings(const ProcessingSettings &settings);
    bool startStreaming();
    bool stopStreaming();
    bool publishProcessedImage(const QImage &image, quint64 frameNumber);
    void appendLog(const QString &level, const QString &message);
    void setStreamError(const QString &message);

signals:
    void processingSettingsRequested(const ProcessingSettings &settings);
    void streamingStarted();
    void streamingStopped();

private:
    AppState m_appState;
    ScanPipeline *m_scanPipeline = nullptr;
};
