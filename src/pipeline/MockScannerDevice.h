#pragma once

#include "core/ScanFrame.h"

#include <QObject>

class QTimer;

class MockScannerDevice : public QObject
{
    Q_OBJECT

public:
    explicit MockScannerDevice(QObject *parent = nullptr);

    bool start();
    bool stop();
    [[nodiscard]] bool isStreaming() const;

signals:
    void frameGenerated(const ScanFrame &frame);
    void streamingChanged(bool isStreaming);

private slots:
    void generateFrame();

private:
    QTimer *m_timer = nullptr;
    quint64 m_frameNumber = 0;
    bool m_isStreaming = false;
};
