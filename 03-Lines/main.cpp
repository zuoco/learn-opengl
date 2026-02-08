#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "canvasprovider.h"

/*
 * 课程内容介绍：
 * 1 光栅化Raster类设计
 * 2 Brensenham算法实现
 * 3 直线算法测试
 */

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // 创建 ImageProvider 和 Controller
    CanvasImageProvider* imageProvider = new CanvasImageProvider();
    CanvasController* controller = new CanvasController(imageProvider);

    engine.addImageProvider("canvas", imageProvider);
    engine.rootContext()->setContextProperty("canvasController", controller);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("LinesQt", "Main");

    return app.exec();
}
