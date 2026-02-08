import QtQuick

/*
 * 课程内容介绍：
 * 1 光栅化Raster类设计
 * 2 Brensenham算法实现
 * 3 直线算法测试
 */

Window {
    id: root
    width: 800
    height: 600
    visible: true
    title: qsTr("GraphicLearning - Lines (Qt)")

    Image {
        id: canvas
        anchors.fill: parent
        source: "image://canvas/frame"
        cache: false
        fillMode: Image.PreserveAspectFit
    }

    Connections {
        target: canvasController
        function onImageReady() {
            // 刷新图像，强制重新加载
            canvas.source = ""
            canvas.source = "image://canvas/frame?" + canvasController.frameCount
        }
    }

    Component.onCompleted: {
        canvasController.startRendering()
    }

    Component.onDestruction: {
        canvasController.stopRendering()
    }
}
