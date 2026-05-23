#pragma once

#include <QDateTime>
#include <QMetaType>
#include <QVector>

struct ScanFrame
{
    int width = 0;
    int height = 0;
    QVector<quint16> pixels;
    quint64 frameNumber = 0;
    QDateTime timestamp = QDateTime::currentDateTimeUtc();

    [[nodiscard]] bool isValid() const
    {
        return width > 0 && height > 0 && pixels.size() == (width * height);
    }
};

Q_DECLARE_METATYPE(ScanFrame)
