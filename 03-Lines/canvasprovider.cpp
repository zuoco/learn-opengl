#include "canvasprovider.h"
#include <cstdlib>
#include <ctime>
#include <QMutexLocker>

// CanvasImageProvider implementation
CanvasImageProvider::CanvasImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{
}

void CanvasImageProvider::setImage(const QImage& image) {
    QMutexLocker locker(&m_mutex);
    m_image = image;
}

QImage CanvasImageProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize) {
    Q_UNUSED(id)
    Q_UNUSED(requestedSize)

    QMutexLocker locker(&m_mutex);
    if (size) {
        *size = m_image.size();
    }
    return m_image;
}

// CanvasController implementation
CanvasController::CanvasController(CanvasImageProvider* provider, QObject* parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
    , m_provider(provider)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    m_image = QImage(m_width, m_height, QImage::Format_RGBA8888);
    m_image.fill(Qt::black);

    sgl->initSurface(m_width, m_height);

    connect(m_timer, &QTimer::timeout, this, &CanvasController::render);
}

CanvasController::~CanvasController() {
    stopRendering();
}

void CanvasController::startRendering() {
    m_timer->start(16); // ~60 FPS
}

void CanvasController::stopRendering() {
    m_timer->stop();
}

void CanvasController::render() {
    sgl->clear();

    int r = 150;
    Point c{ 400, 300, RGBA(255, 0, 0, 255) };

    for (float i = 0; i < 360; i += 10) {
        float radian = DEG2RAD(i);
        int x = r * sin(radian) + c.x;
        int y = r * cos(radian) + c.y;

        Point pt{ x, y, RGBA(static_cast<byte>(std::rand() % 255),
                             static_cast<byte>(std::rand() % 255),
                             static_cast<byte>(std::rand() % 255), 255) };

        sgl->drawLine(c, pt);
    }

    // 将 FrameBuffer 数据复制到 QImage
    FrameBuffer* fb = sgl->getFrameBuffer();
    if (fb && fb->mColorBuffer) {
        for (uint32_t y = 0; y < fb->mHeight; ++y) {
            for (uint32_t x = 0; x < fb->mWidth; ++x) {
                uint32_t srcIndex = y * fb->mWidth + x;
                // Qt Y轴从上到下，原始代码 Y轴从下到上，需要翻转
                uint32_t dstY = fb->mHeight - 1 - y;

                RGBA& pixel = fb->mColorBuffer[srcIndex];
                m_image.setPixelColor(x, dstY, QColor(pixel.mR, pixel.mG, pixel.mB, pixel.mA));
            }
        }
    }

    m_provider->setImage(m_image);

    m_frameCount++;
    emit frameCountChanged();
    emit imageReady();
}
