#pragma once

#include <QImage>
#include <QWidget>

class ScanViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScanViewWidget(QWidget *parent = nullptr);

    void setImage(const QImage &image);
    void setPlaceholderText(const QString &text);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QImage m_image;
    QString m_placeholderText;
};
