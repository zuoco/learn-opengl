#pragma once

#include <QObject>
#include <QImage>
#include <QQuickImageProvider>
#include <QTimer>
#include <QMutex>
#include "gpu/gpu.h"

class CanvasImageProvider : public QQuickImageProvider {
public:
    CanvasImageProvider();
    QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override;
    void setImage(const QImage& image);

private:
    QImage m_image;
    QMutex m_mutex;
};

class CanvasController : public QObject {
    Q_OBJECT
    Q_PROPERTY(int frameCount READ frameCount NOTIFY frameCountChanged)

public:
    explicit CanvasController(CanvasImageProvider* provider, QObject* parent = nullptr);
    ~CanvasController() override;

    int frameCount() const { return m_frameCount; }

    Q_INVOKABLE void startRendering();
    Q_INVOKABLE void stopRendering();

signals:
    void frameCountChanged();
    void imageReady();

private slots:
    void render();

private:
    int m_width = 800;
    int m_height = 600;
    int m_frameCount = 0;
    QTimer* m_timer;
    QImage m_image;
    CanvasImageProvider* m_provider;
};
