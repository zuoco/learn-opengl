#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QImage>
#include <QTimer>
#include <QMutex>
#include <QQuickImageProvider>

class Application;

// 图像提供者，用于在 QML 中显示渲染结果
class FrameImageProvider : public QQuickImageProvider {
public:
    FrameImageProvider();
    QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override;

    void updateImage(const QImage& image);

private:
    QImage mCurrentImage;
    mutable QMutex mMutex;
};

class Application : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(int width READ getWidth CONSTANT)
    Q_PROPERTY(int height READ getHeight CONSTANT)

public:
    static Application& getInstance();
    static Application* create(QQmlEngine* engine, QJSEngine* scriptEngine);

    Q_INVOKABLE void quit();
    Q_INVOKABLE QString getApplicationInfo() const;

    int getWidth() const { return mWidth; }
    int getHeight() const { return mHeight; }

    void setSize(int width, int height);
    FrameImageProvider* getImageProvider() { return mImageProvider; }
    void setImageProvider(FrameImageProvider* provider) { mImageProvider = provider; }

    // 渲染相关
    void render();
    void updateFrame();

signals:
    void applicationClosing();
    void frameUpdated();

private:
    Application(QObject* parent = nullptr);
    ~Application() override = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    bool mAlive{true};
    int mWidth{800};
    int mHeight{600};

    FrameImageProvider* mImageProvider{nullptr};
};

#define sglApp (&Application::getInstance())
