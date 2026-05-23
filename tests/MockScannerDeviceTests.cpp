#include "pipeline/MockScannerDevice.h"

#include <QSignalSpy>
#include <QtTest/QTest>

class MockScannerDeviceTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void emitsFramesWhenStarted();
    void stopsCleanly();
};

void MockScannerDeviceTests::initTestCase()
{
    qRegisterMetaType<ScanFrame>("ScanFrame");
}

void MockScannerDeviceTests::emitsFramesWhenStarted()
{
    MockScannerDevice device;
    QSignalSpy frameSpy(&device, &MockScannerDevice::frameGenerated);

    QVERIFY(device.start());
    QTRY_VERIFY(frameSpy.count() >= 2);

    const auto firstArguments = frameSpy.takeFirst();
    const ScanFrame frame = qvariant_cast<ScanFrame>(firstArguments.at(0));
    QVERIFY(frame.isValid());
    QVERIFY(device.isStreaming());
}

void MockScannerDeviceTests::stopsCleanly()
{
    MockScannerDevice device;
    QVERIFY(device.start());
    QVERIFY(device.stop());
    QVERIFY(!device.isStreaming());
    QVERIFY(!device.stop());
}

QTEST_APPLESS_MAIN(MockScannerDeviceTests)

#include "MockScannerDeviceTests.moc"
