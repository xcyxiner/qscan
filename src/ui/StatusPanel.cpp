#include "ui/StatusPanel.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>

namespace {
QString connectionStateText(AppState::ConnectionState state)
{
    switch (state) {
    case AppState::ConnectionState::Disconnected:
        return QStringLiteral("未连接");
    case AppState::ConnectionState::Connecting:
        return QStringLiteral("连接中");
    case AppState::ConnectionState::Connected:
        return QStringLiteral("已连接");
    }

    return QStringLiteral("未知");
}

QString streamStateText(AppState::StreamState state)
{
    switch (state) {
    case AppState::StreamState::Idle:
        return QStringLiteral("空闲");
    case AppState::StreamState::Starting:
        return QStringLiteral("启动中");
    case AppState::StreamState::Streaming:
        return QStringLiteral("推流中");
    case AppState::StreamState::Stopping:
        return QStringLiteral("停止中");
    case AppState::StreamState::Error:
        return QStringLiteral("异常");
    }

    return QStringLiteral("未知");
}
}

StatusPanel::StatusPanel(QWidget *parent)
    : QWidget(parent)
    , m_connectionValueLabel(new QLabel(QStringLiteral("未连接"), this))
    , m_streamValueLabel(new QLabel(QStringLiteral("空闲"), this))
    , m_frameValueLabel(new QLabel(QStringLiteral("0"), this))
{
    auto *layout = new QVBoxLayout(this);
    auto *groupBox = new QGroupBox(QStringLiteral("系统状态"), this);
    auto *formLayout = new QFormLayout(groupBox);

    formLayout->addRow(QStringLiteral("连接状态"), m_connectionValueLabel);
    formLayout->addRow(QStringLiteral("流状态"), m_streamValueLabel);
    formLayout->addRow(QStringLiteral("最新帧号"), m_frameValueLabel);

    layout->addWidget(groupBox);
}

void StatusPanel::setConnectionState(AppState::ConnectionState state)
{
    m_connectionValueLabel->setText(connectionStateText(state));
}

void StatusPanel::setStreamState(AppState::StreamState state)
{
    m_streamValueLabel->setText(streamStateText(state));
}

void StatusPanel::setFrameNumber(quint64 frameNumber)
{
    m_frameValueLabel->setText(QString::number(frameNumber));
}
