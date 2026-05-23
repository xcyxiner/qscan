#include "pipeline/ImageProcessor.h"

#include <QtGlobal>

namespace {
int clampToByte(double value)
{
    return qBound(0, static_cast<int>(value), 255);
}
}

QImage ImageProcessor::process(const ScanFrame &frame, const ProcessingSettings &settings) const
{
    if (!frame.isValid()) {
        return {};
    }

    QVector<int> grayscale(frame.width * frame.height, 0);
    const double contrastScale = 1.0 + (static_cast<double>(settings.contrast) / 100.0);

    for (int index = 0; index < frame.pixels.size(); ++index) {
        const double normalized = static_cast<double>(frame.pixels[index]) / 65535.0;
        const double centered = (normalized - 0.5) * contrastScale + 0.5;
        const double adjusted = centered * 255.0 + static_cast<double>(settings.brightness);
        grayscale[index] = clampToByte(adjusted);
    }

    if (settings.edgeEnhancementEnabled) {
        QVector<int> sharpened = grayscale;
        for (int y = 1; y < frame.height - 1; ++y) {
            for (int x = 1; x < frame.width - 1; ++x) {
                const int index = y * frame.width + x;
                const int neighbors = grayscale[index - 1] + grayscale[index + 1]
                    + grayscale[index - frame.width] + grayscale[index + frame.width];
                const double enhanced = static_cast<double>(grayscale[index]) * 1.7 - static_cast<double>(neighbors) * 0.175;
                sharpened[index] = clampToByte(enhanced);
            }
        }
        grayscale.swap(sharpened);
    }

    QImage image(frame.width, frame.height, QImage::Format_Grayscale8);
    for (int y = 0; y < frame.height; ++y) {
        uchar *scanLine = image.scanLine(y);
        for (int x = 0; x < frame.width; ++x) {
            scanLine[x] = static_cast<uchar>(grayscale[y * frame.width + x]);
        }
    }

    return image;
}
