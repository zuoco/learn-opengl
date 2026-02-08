#include "application.h"
#include "../gpu/gpu.h"
#include <QCoreApplication>
#include <QMutexLocker>
#include <cstdlib>

// FrameImageProvider 实现
FrameImageProvider::FrameImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{
    // 初始化一个默认的黑色图像
    mCurrentImage = QImage(800, 600, QImage::Format_RGBA8888);
    mCurrentImage.fill(Qt::black);
}

QImage FrameImageProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
    Q_UNUSED(id)
    Q_UNUSED(requestedSize)
    
    QMutexLocker locker(&mMutex);
    
    if (size) {
        *size = mCurrentImage.size();
    }
    
    return mCurrentImage;
}

void FrameImageProvider::updateImage(const QImage& image)
{
    QMutexLocker locker(&mMutex);
    mCurrentImage = image;
}

// Application 实现
Application::Application(QObject* parent)
    : QObject(parent)
    , mAlive(true)
    , mWidth(800)
    , mHeight(600)
    , mImageProvider(nullptr)
{
}

Application& Application::getInstance()
{
    static Application instance;
    return instance;
}

Application* Application::create(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    
    Application* instance = &getInstance();
    QQmlEngine::setObjectOwnership(instance, QQmlEngine::CppOwnership);
    return instance;
}

void Application::quit()
{
    mAlive = false;
    emit applicationClosing();
    QCoreApplication::quit();
}

QString Application::getApplicationInfo() const
{
    return QStringLiteral("Qt Application - Soft Renderer (GDI to Qt-QML)");
}

void Application::setSize(int width, int height)
{
    mWidth = width;
    mHeight = height;
}

void Application::render()
{
    if (!mAlive) return;
    
    // 清除画布
    sgl->clear();
    
    // 渲染随机噪点（保持原有功能）
    for (int i = 0; i < mWidth; ++i) {
        for (int j = 0; j < mHeight; ++j) {
            uint32_t v = std::rand() % 255;
            RGBA color(v, v, v, v);
            sgl->drawPoint(i, j, color);
        }
    }
    
    // 更新帧
    updateFrame();
}

void Application::updateFrame()
{
    FrameBuffer* fb = sgl->getFrameBuffer();
    if (!fb) return;
    
    // 将 RGBA 缓冲区转换为 QImage
    // 注意：需要垂直翻转图像，因为 GPU 从左下角开始绘制
    QImage image(mWidth, mHeight, QImage::Format_RGBA8888);
    
    for (int y = 0; y < mHeight; ++y) {
        // 翻转 Y 坐标
        int srcY = mHeight - 1 - y;
        for (int x = 0; x < mWidth; ++x) {
            int srcPos = srcY * mWidth + x;
            RGBA& pixel = fb->mColorBuffer[srcPos];
            image.setPixelColor(x, y, QColor(pixel.mR, pixel.mG, pixel.mB, pixel.mA));
        }
    }
    
    mImageProvider->updateImage(image);
    emit frameUpdated();
}
