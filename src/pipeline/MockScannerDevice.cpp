#include "pipeline/MockScannerDevice.h"

#include <QTimer>

#include <algorithm>
#include <cmath>

namespace {
constexpr int kFrameWidth = 320;
constexpr int kFrameHeight = 192;
constexpr int kFrameIntervalMs = 50;
}

MockScannerDevice::MockScannerDevice(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
{
    m_timer->setInterval(kFrameIntervalMs);
    connect(m_timer, &QTimer::timeout, this, &MockScannerDevice::generateFrame);
}

bool MockScannerDevice::start()
{
    if (m_isStreaming) {
        return false;
    }

    m_isStreaming = true;
    m_frameNumber = 0;
    m_timer->start();
    emit streamingChanged(true);
    return true;
}

bool MockScannerDevice::stop()
{
    if (!m_isStreaming) {
        return false;
    }

    m_timer->stop();
    m_isStreaming = false;
    emit streamingChanged(false);
    return true;
}

bool MockScannerDevice::isStreaming() const
{
    return m_isStreaming;
}

void MockScannerDevice::generateFrame()
{
    ScanFrame frame;
    frame.width = kFrameWidth;
    frame.height = kFrameHeight;
    frame.frameNumber = ++m_frameNumber;
    frame.timestamp = QDateTime::currentDateTimeUtc();
    frame.pixels.resize(frame.width * frame.height);

    const double horizontalPhase = static_cast<double>(m_frameNumber % frame.width) / static_cast<double>(frame.width);
    const double verticalPhase = static_cast<double>((m_frameNumber * 2) % frame.height) / static_cast<double>(frame.height);

    for (int y = 0; y < frame.height; ++y) {
        for (int x = 0; x < frame.width; ++x) {
            const double wave = (std::sin((static_cast<double>(x) / frame.width + horizontalPhase) * 6.283185307179586) + 1.0) * 0.5;
            const double sweep = (std::sin((static_cast<double>(y) / frame.height + verticalPhase) * 12.566370614359172) + 1.0) * 0.5;
            const double stripe = ((x + static_cast<int>(m_frameNumber * 3)) % 48) < 24 ? 0.22 : 0.0;
            const double hotspotX = static_cast<double>((m_frameNumber * 5) % frame.width);
            const double hotspotY = static_cast<double>((m_frameNumber * 3) % frame.height);
            const double dx = static_cast<double>(x) - hotspotX;
            const double dy = static_cast<double>(y) - hotspotY;
            const double hotspot = std::exp(-(dx * dx + dy * dy) / 2200.0);
            const double value = std::clamp(0.45 * wave + 0.25 * sweep + stripe + 0.35 * hotspot, 0.0, 1.0);
            frame.pixels[y * frame.width + x] = static_cast<quint16>(value * 65535.0);
        }
    }

    emit frameGenerated(frame);
}
