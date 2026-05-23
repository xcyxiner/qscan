#include "ui/ScanViewWidget.h"

#include <QPainter>

ScanViewWidget::ScanViewWidget(QWidget *parent)
    : QWidget(parent)
    , m_placeholderText(QStringLiteral("等待扫描数据"))
{
    setMinimumSize(640, 360);
}

void ScanViewWidget::setImage(const QImage &image)
{
    m_image = image;
    update();
}

void ScanViewWidget::setPlaceholderText(const QString &text)
{
    m_placeholderText = text;
    update();
}

void ScanViewWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.fillRect(rect(), QColor(12, 14, 18));

    if (m_image.isNull()) {
        painter.setPen(QColor(180, 186, 196));
        painter.drawText(rect(), Qt::AlignCenter, m_placeholderText);
        return;
    }

    const QImage scaled = m_image.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    const QPoint topLeft((width() - scaled.width()) / 2, (height() - scaled.height()) / 2);
    painter.drawImage(topLeft, scaled);
}
