#pragma once

#include "core/ProcessingSettings.h"

#include <QWidget>

class QPushButton;
class QSlider;
class QCheckBox;

class ControlPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ControlPanel(QWidget *parent = nullptr);

    void setSettings(const ProcessingSettings &settings);

signals:
    void settingsChanged(const ProcessingSettings &settings);
    void startRequested();
    void stopRequested();

private slots:
    void emitSettingsFromControls();

private:
    [[nodiscard]] ProcessingSettings currentSettings() const;

    QPushButton *m_startButton = nullptr;
    QPushButton *m_stopButton = nullptr;
    QSlider *m_brightnessSlider = nullptr;
    QSlider *m_contrastSlider = nullptr;
    QCheckBox *m_edgeEnhancementCheckBox = nullptr;
    bool m_isApplyingState = false;
};
