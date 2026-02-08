import QtQuick

Window {
    id: mainWindow
    
    width: 800
    height: 600
    visible: true
    title: qsTr("Soft Renderer - GDI to Qt-QML")
    color: "black"
    
    // 显示渲染结果的图像
    Image {
        id: frameImage
        anchors.fill: parent
        cache: false
        source: "image://frameProvider/frame"
        fillMode: Image.PreserveAspectFit
    }
    
    // 定时器刷新图像
    Timer {
        id: refreshTimer
        interval: 16  // 约60fps
        running: true
        repeat: true
        onTriggered: {
            frameImage.source = ""
            frameImage.source = "image://frameProvider/frame?" + Date.now()
        }
    }
}
