import QtQuick 2.4

import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

Window {
    id: window
    visible: true
    width: 320
    height: 240
    title: TITLE
        TextEdit {
            textFormat: TextEdit.RichText
            text: DESCRIPTION
            readOnly: true
            anchors.fill: parent
            wrapMode: Text.WordWrap
            selectByMouse: true
        }

}