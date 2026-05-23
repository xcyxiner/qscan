#include "pipeline/ScanPipeline.h"

#include "pipeline/ImageProcessor.h"
#include "pipeline/MockScannerDevice.h"

#include <QThread>

class PipelineWorker : public QObject
{
    Q_OBJECT

public:
    explicit PipelineWorker(QObject *parent = nullptr)
        : QObject(parent)
    {
        connect(&m_device, &MockScannerDevice::frameGenerated, this, &PipelineWorker::processFrame);
    }

    void moveOwnedObjectsToThread(QThread *targetThread)
    {
        m_device.moveToThread(targetThread);
    }

public slots:
    void start()
    {
        emit connectionStateChanged(AppState::ConnectionState::Connecting);
        emit streamStateChanged(AppState::StreamState::Starting);

        if (m_device.start()) {
            emit connectionStateChanged(AppState::ConnectionState::Connected);
            emit streamStateChanged(AppState::StreamState::Streaming);
            return;
        }

        emit errorOccurred(QStringLiteral("Mock scanner is already running"));
    }

    void stop()
    {
        if (!m_device.isStreaming()) {
            emit connectionStateChanged(AppState::ConnectionState::Disconnected);
            emit streamStateChanged(AppState::StreamState::Idle);
            return;
        }

        emit streamStateChanged(AppState::StreamState::Stopping);
        m_device.stop();
        emit streamStateChanged(AppState::StreamState::Idle);
        emit connectionStateChanged(AppState::ConnectionState::Disconnected);
    }

    void updateSettings(const ProcessingSettings &settings)
    {
        m_settings = settings;
    }

private slots:
    void processFrame(const ScanFrame &frame)
    {
        const QImage image = m_processor.process(frame, m_settings);
        emit frameReady(image, frame.frameNumber);
    }

signals:
    void frameReady(const QImage &image, quint64 frameNumber);
    void connectionStateChanged(AppState::ConnectionState state);
    void streamStateChanged(AppState::StreamState state);
    void errorOccurred(const QString &message);

private:
    MockScannerDevice m_device;
    ImageProcessor m_processor;
    ProcessingSettings m_settings;
};

ScanPipeline::ScanPipeline(QObject *parent)
    : QObject(parent)
    , m_workerThread(new QThread(this))
    , m_worker(new PipelineWorker)
{
    qRegisterMetaType<ProcessingSettings>("ProcessingSettings");
    qRegisterMetaType<ScanFrame>("ScanFrame");
    qRegisterMetaType<AppState::ConnectionState>("AppState::ConnectionState");
    qRegisterMetaType<AppState::StreamState>("AppState::StreamState");

    m_worker->moveToThread(m_workerThread);

    auto *typedWorker = static_cast<PipelineWorker *>(m_worker);
    typedWorker->moveOwnedObjectsToThread(m_workerThread);

    connect(m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);
    connect(this, &ScanPipeline::startRequested, typedWorker, &PipelineWorker::start);
    connect(this, &ScanPipeline::stopRequested, typedWorker, &PipelineWorker::stop);
    connect(this, &ScanPipeline::settingsUpdateRequested, typedWorker, &PipelineWorker::updateSettings);
    connect(typedWorker, &PipelineWorker::frameReady, this, &ScanPipeline::frameReady);
    connect(typedWorker, &PipelineWorker::connectionStateChanged, this, &ScanPipeline::connectionStateChanged);
    connect(typedWorker, &PipelineWorker::streamStateChanged, this, &ScanPipeline::streamStateChanged);
    connect(typedWorker, &PipelineWorker::errorOccurred, this, &ScanPipeline::errorOccurred);

    m_workerThread->start();
}

ScanPipeline::~ScanPipeline()
{
    if (m_workerThread->isRunning()) {
        stop();
        m_workerThread->quit();
        m_workerThread->wait();
    }
}

void ScanPipeline::start()
{
    emit startRequested();
}

void ScanPipeline::stop()
{
    emit stopRequested();
}

void ScanPipeline::updateSettings(const ProcessingSettings &settings)
{
    emit settingsUpdateRequested(settings);
}

#include "ScanPipeline.moc"
