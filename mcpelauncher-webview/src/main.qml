import QtQuick 2.2
import QtQuick.Controls 1.1
import QtWebEngine 1.5
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2

ApplicationWindow {
    visible: true
    x: initialX
    y: initialY
    width: initialWidth
    height: initialHeight
    title: webEngineView.title

    WebEngineView {
        id: webEngineView
        anchors.fill: parent
        url: startUrl
    }
}