#include "pipeline/ImageProcessor.h"

#include <QtTest/QTest>

class ImageProcessorTests : public QObject
{
    Q_OBJECT

private slots:
    void appliesBrightnessAndContrastChanges();
    void handlesZeroValuedFrames();
    void appliesEdgeEnhancementWhenRequested();
};

namespace {
ScanFrame makeFrame(int width, int height)
{
    ScanFrame frame;
    frame.width = width;
    frame.height = height;
    frame.pixels.resize(width * height);
    for (int index = 0; index < frame.pixels.size(); ++index) {
        frame.pixels[index] = static_cast<quint16>((index * 65535) / qMax(1, frame.pixels.size() - 1));
    }
    return frame;
}
}

void ImageProcessorTests::appliesBrightnessAndContrastChanges()
{
    ImageProcessor processor;
    const ScanFrame frame = makeFrame(4, 4);

    const QImage baseline = processor.process(frame, ProcessingSettings{});

    ProcessingSettings adjusted;
    adjusted.brightness = 25;
    adjusted.contrast = 20;

    const QImage changed = processor.process(frame, adjusted);

    QVERIFY(!baseline.isNull());
    QVERIFY(!changed.isNull());
    QVERIFY(baseline.pixelColor(2, 2) != changed.pixelColor(2, 2));
}

void ImageProcessorTests::handlesZeroValuedFrames()
{
    ImageProcessor processor;
    ScanFrame frame;
    frame.width = 8;
    frame.height = 8;
    frame.pixels.fill(0, frame.width * frame.height);

    const QImage image = processor.process(frame, ProcessingSettings{});

    QVERIFY(!image.isNull());
    QCOMPARE(image.width(), 8);
    QCOMPARE(image.height(), 8);
}

void ImageProcessorTests::appliesEdgeEnhancementWhenRequested()
{
    ImageProcessor processor;
    const ScanFrame frame = makeFrame(6, 6);

    ProcessingSettings settings;
    settings.edgeEnhancementEnabled = true;

    const QImage baseline = processor.process(frame, ProcessingSettings{});
    const QImage enhanced = processor.process(frame, settings);

    QVERIFY(!enhanced.isNull());
    QVERIFY(baseline.pixelColor(3, 3) != enhanced.pixelColor(3, 3));
}

QTEST_APPLESS_MAIN(ImageProcessorTests)

#include "ImageProcessorTests.moc"
