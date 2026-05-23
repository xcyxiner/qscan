#include "ui/LogPanel.h"

#include <QGroupBox>
#include <QListWidget>
#include <QVBoxLayout>

LogPanel::LogPanel(QWidget *parent)
    : QWidget(parent)
    , m_listWidget(new QListWidget(this))
{
    auto *layout = new QVBoxLayout(this);
    auto *groupBox = new QGroupBox(QStringLiteral("运行日志"), this);
    auto *groupLayout = new QVBoxLayout(groupBox);

    m_listWidget->setSelectionMode(QAbstractItemView::NoSelection);
    groupLayout->addWidget(m_listWidget);
    layout->addWidget(groupBox);
}

void LogPanel::appendEntry(const LogEntry &entry)
{
    const QString line = QStringLiteral("[%1] %2 %3")
                             .arg(entry.timestamp.toLocalTime().toString(QStringLiteral("HH:mm:ss.zzz")))
                             .arg(entry.level.toUpper())
                             .arg(entry.message);
    m_listWidget->addItem(line);
    m_listWidget->scrollToBottom();
}
