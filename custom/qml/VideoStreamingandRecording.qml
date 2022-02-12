import QtQuick 2.0
import QtMultimedia 5.6
import QGroundControl   1.0
import QGroundControl.Controls     1.0


Item {
     id: root
     Rectangle {
       height: 100
       width: 100
       anchors.left: parent.left
       anchors.leftMargin: ScreenTools.defaultFontPixelWidth * 2.5
       anchors.bottom:         parent.bottom
       anchors.bottomMargin:    ScreenTools.defaultFontPixelWidth + 180
       border.color: "white"
       radius: 8
       color: "#801F1F1F"
       visible: true
       QGCLabel {
         text: ""

       }
     }
}
