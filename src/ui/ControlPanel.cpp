#include "ui/ControlPanel.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>

ControlPanel::ControlPanel(QWidget *parent)
    : QWidget(parent)
    , m_startButton(new QPushButton(QStringLiteral("启动模拟流"), this))
    , m_stopButton(new QPushButton(QStringLiteral("停止模拟流"), this))
    , m_brightnessSlider(new QSlider(Qt::Horizontal, this))
    , m_contrastSlider(new QSlider(Qt::Horizontal, this))
    , m_edgeEnhancementCheckBox(new QCheckBox(QStringLiteral("启用锐化增强"), this))
{
    auto *layout = new QVBoxLayout(this);
    auto *groupBox = new QGroupBox(QStringLiteral("控制面板"), this);
    auto *formLayout = new QFormLayout(groupBox);

    m_brightnessSlider->setRange(-100, 100);
    m_contrastSlider->setRange(-100, 100);

    formLayout->addRow(QStringLiteral("亮度"), m_brightnessSlider);
    formLayout->addRow(QStringLiteral("对比度"), m_contrastSlider);
    formLayout->addRow(QString(), m_edgeEnhancementCheckBox);

    layout->addWidget(groupBox);
    layout->addWidget(m_startButton);
    layout->addWidget(m_stopButton);
    layout->addStretch();

    connect(m_startButton, &QPushButton::clicked, this, &ControlPanel::startRequested);
    connect(m_stopButton, &QPushButton::clicked, this, &ControlPanel::stopRequested);
    connect(m_brightnessSlider, &QSlider::valueChanged, this, &ControlPanel::emitSettingsFromControls);
    connect(m_contrastSlider, &QSlider::valueChanged, this, &ControlPanel::emitSettingsFromControls);
    connect(m_edgeEnhancementCheckBox, &QCheckBox::toggled, this, &ControlPanel::emitSettingsFromControls);
}

void ControlPanel::setSettings(const ProcessingSettings &settings)
{
    m_isApplyingState = true;
    m_brightnessSlider->setValue(settings.brightness);
    m_contrastSlider->setValue(settings.contrast);
    m_edgeEnhancementCheckBox->setChecked(settings.edgeEnhancementEnabled);
    m_isApplyingState = false;
}

void ControlPanel::emitSettingsFromControls()
{
    if (m_isApplyingState) {
        return;
    }

    emit settingsChanged(currentSettings());
}

ProcessingSettings ControlPanel::currentSettings() const
{
    ProcessingSettings settings;
    settings.brightness = m_brightnessSlider->value();
    settings.contrast = m_contrastSlider->value();
    settings.edgeEnhancementEnabled = m_edgeEnhancementCheckBox->isChecked();
    return settings;
}
