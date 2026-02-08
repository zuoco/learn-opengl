#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QTimer>
#include "application/application.h"
#include "gpu/gpu.h"

/*
* 课程内容介绍：
* 1 GDI 绘制环境搭建 -> Qt-QML 环境
* 2 搭建模拟GPU的代码架构
* 3 点的绘制实践
*/

int main(int argc, char *argv[])
{
    QGuiApplication qtApp(argc, argv);
    
    // 获取 Application 单例并初始化
    Application& appInstance = Application::getInstance();
    
    // 初始化 GPU
    sgl->initSurface(appInstance.getWidth(), appInstance.getHeight());
    
    QQmlApplicationEngine engine;
    
    // 创建并注册图像提供者（Qt 会接管所有权）
    FrameImageProvider* imageProvider = new FrameImageProvider();
    appInstance.setImageProvider(imageProvider);
    engine.addImageProvider(QStringLiteral("frameProvider"), imageProvider);
    
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &qtApp, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    
    engine.load(url);
    
    // 创建渲染定时器
    QTimer renderTimer;
    QObject::connect(&renderTimer, &QTimer::timeout, [&appInstance]() {
        appInstance.render();
    });
    renderTimer.start(16); // 约60fps
    
    return qtApp.exec();
}
