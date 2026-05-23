#pragma once

#include "core/ProcessingSettings.h"
#include "core/ScanFrame.h"

#include <QImage>

class ImageProcessor
{
public:
    [[nodiscard]] QImage process(const ScanFrame &frame, const ProcessingSettings &settings) const;
};
