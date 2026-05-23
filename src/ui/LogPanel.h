#pragma once

#include "app/AppState.h"

#include <QWidget>

class QListWidget;

class LogPanel : public QWidget
{
    Q_OBJECT

public:
    explicit LogPanel(QWidget *parent = nullptr);

    void appendEntry(const LogEntry &entry);

private:
    QListWidget *m_listWidget = nullptr;
};
