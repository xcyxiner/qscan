#include "app/ApplicationController.h"

#include <QtTest/QTest>

class AppStateIntegrationTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void publishesFramesToApplicationState();
    void recordsStatusAndLogsAcrossStartStop();
};

void AppStateIntegrationTests::initTestCase()
{
    qRegisterMetaType<AppState::ConnectionState>("AppState::ConnectionState");
    qRegisterMetaType<AppState::StreamState>("AppState::StreamState");
}

void AppStateIntegrationTests::publishesFramesToApplicationState()
{
    ApplicationController controller;

    QVERIFY(controller.startStreaming());

    QTRY_VERIFY(controller.appState()->currentFrameNumber() > 0);
    QTRY_VERIFY(!controller.appState()->currentImage().isNull());
    QCOMPARE(controller.appState()->connectionState(), AppState::ConnectionState::Connected);
    QCOMPARE(controller.appState()->streamState(), AppState::StreamState::Streaming);
}

void AppStateIntegrationTests::recordsStatusAndLogsAcrossStartStop()
{
    ApplicationController controller;

    QVERIFY(controller.startStreaming());
    QTRY_VERIFY(controller.appState()->currentFrameNumber() > 0);
    QVERIFY(controller.stopStreaming());
    QTRY_COMPARE(controller.appState()->streamState(), AppState::StreamState::Idle);
    QTRY_COMPARE(controller.appState()->connectionState(), AppState::ConnectionState::Disconnected);
    QVERIFY(controller.appState()->logEntries().size() >= 3);
}

QTEST_MAIN(AppStateIntegrationTests)

#include "AppStateIntegrationTests.moc"