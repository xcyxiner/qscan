#pragma once

#include "app/AppState.h"

#include <QWidget>

class QLabel;

class StatusPanel : public QWidget
{
    Q_OBJECT

public:
    explicit StatusPanel(QWidget *parent = nullptr);

    void setConnectionState(AppState::ConnectionState state);
    void setStreamState(AppState::StreamState state);
    void setFrameNumber(quint64 frameNumber);

private:
    QLabel *m_connectionValueLabel = nullptr;
    QLabel *m_streamValueLabel = nullptr;
    QLabel *m_frameValueLabel = nullptr;
};
