#pragma once

#include <QMetaType>

struct ProcessingSettings
{
    int brightness = 0;
    int contrast = 0;
    bool edgeEnhancementEnabled = false;

    [[nodiscard]] bool operator==(const ProcessingSettings &other) const
    {
        return brightness == other.brightness
            && contrast == other.contrast
            && edgeEnhancementEnabled == other.edgeEnhancementEnabled;
    }

    [[nodiscard]] bool operator!=(const ProcessingSettings &other) const
    {
        return !(*this == other);
    }
};

Q_DECLARE_METATYPE(ProcessingSettings)
