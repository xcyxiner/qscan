#pragma once

#include "app/AppState.h"
#include "core/ProcessingSettings.h"

#include <QObject>

class QThread;

class ScanPipeline : public QObject
{
    Q_OBJECT

public:
    explicit ScanPipeline(QObject *parent = nullptr);
    ~ScanPipeline() override;

    void start();
    void stop();
    void updateSettings(const ProcessingSettings &settings);

signals:
    void frameReady(const QImage &image, quint64 frameNumber);
    void connectionStateChanged(AppState::ConnectionState state);
    void streamStateChanged(AppState::StreamState state);
    void errorOccurred(const QString &message);

private:
    QThread *m_workerThread = nullptr;
    QObject *m_worker = nullptr;

signals:
    void startRequested();
    void stopRequested();
    void settingsUpdateRequested(const ProcessingSettings &settings);
};
