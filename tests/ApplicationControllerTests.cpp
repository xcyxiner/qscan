#include "app/ApplicationController.h"

#include <QSignalSpy>
#include <QtTest/QTest>

class ApplicationControllerTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void initializesWithExpectedDefaultState();
    void updatesProcessingSettingsOnce();
    void doesNotDuplicateStateForRepeatedSettings();
    void stopsSafelyBeforeStreamingStarts();
    void exposesObservableStreamingStateTransitions();
};

void ApplicationControllerTests::initTestCase()
{
    qRegisterMetaType<ProcessingSettings>("ProcessingSettings");
    qRegisterMetaType<AppState::ConnectionState>("AppState::ConnectionState");
    qRegisterMetaType<AppState::StreamState>("AppState::StreamState");
    qRegisterMetaType<LogEntry>("LogEntry");
}

void ApplicationControllerTests::initializesWithExpectedDefaultState()
{
    ApplicationController controller;

    QCOMPARE(controller.appState()->connectionState(), AppState::ConnectionState::Disconnected);
    QCOMPARE(controller.appState()->streamState(), AppState::StreamState::Idle);
    QCOMPARE(controller.appState()->currentFrameNumber(), 0ULL);
    QVERIFY(controller.appState()->currentImage().isNull());
    QCOMPARE(controller.processingSettings(), ProcessingSettings{});
    QCOMPARE(controller.appState()->logEntries().size(), 1);
    QCOMPARE(controller.appState()->logEntries().constFirst().message, QStringLiteral("QScan controller initialized"));
}

void ApplicationControllerTests::updatesProcessingSettingsOnce()
{
    ApplicationController controller;
    QSignalSpy settingsSpy(controller.appState(), &AppState::processingSettingsChanged);

    ProcessingSettings settings;
    settings.brightness = 15;
    settings.contrast = 10;
    settings.edgeEnhancementEnabled = true;

    QVERIFY(controller.updateProcessingSettings(settings));
    QCOMPARE(settingsSpy.count(), 1);
    QCOMPARE(controller.processingSettings(), settings);
    QCOMPARE(controller.appState()->logEntries().size(), 2);
}

void ApplicationControllerTests::doesNotDuplicateStateForRepeatedSettings()
{
    ApplicationController controller;
    QSignalSpy settingsSpy(controller.appState(), &AppState::processingSettingsChanged);

    ProcessingSettings settings;
    settings.brightness = 5;
    settings.contrast = 12;
    settings.edgeEnhancementEnabled = false;

    QVERIFY(controller.updateProcessingSettings(settings));
    QVERIFY(!controller.updateProcessingSettings(settings));
    QCOMPARE(settingsSpy.count(), 1);
    QCOMPARE(controller.appState()->logEntries().size(), 2);
}

void ApplicationControllerTests::stopsSafelyBeforeStreamingStarts()
{
    ApplicationController controller;

    QVERIFY(!controller.stopStreaming());
    QCOMPARE(controller.appState()->connectionState(), AppState::ConnectionState::Disconnected);
    QCOMPARE(controller.appState()->streamState(), AppState::StreamState::Idle);
    QCOMPARE(controller.appState()->logEntries().size(), 1);
}

void ApplicationControllerTests::exposesObservableStreamingStateTransitions()
{
    ApplicationController controller;
    QSignalSpy connectionSpy(controller.appState(), &AppState::connectionStateChanged);
    QSignalSpy streamSpy(controller.appState(), &AppState::streamStateChanged);

    QVERIFY(controller.startStreaming());

    QTRY_VERIFY(connectionSpy.count() >= 2);
    QTRY_VERIFY(streamSpy.count() >= 2);
    QTRY_COMPARE(controller.appState()->connectionState(), AppState::ConnectionState::Connected);
    QTRY_COMPARE(controller.appState()->streamState(), AppState::StreamState::Streaming);
}

QTEST_MAIN(ApplicationControllerTests)

#include "ApplicationControllerTests.moc"