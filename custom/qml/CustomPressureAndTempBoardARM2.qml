import QtQuick 2.0

import QGroundControl               1.0
import QGroundControl.Airspace      1.0
import QGroundControl.Controllers   1.0
import QGroundControl.Controls      1.0
import QGroundControl.FlightDisplay 1.0
import QGroundControl.FlightMap     1.0
import QGroundControl.Palette       1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Vehicle       1.0
import QGroundControl.FactSystem    1.0
import QGroundControl.FactControls  1.0

Item {
    anchors.fill: parent

    property var _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle

    property real _pressueOnBoardARM2:  _activeVehicle.kellerPa3Pressure
    property real _tempOnBoardARM2:  _activeVehicle.kellerPa3Temp

    Rectangle{
        id: rectangle
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: ScreenTools.defaultFontPixelWidth + 7
        anchors.bottom:         parent.bottom
        anchors.bottomMargin:    ScreenTools.defaultFontPixelWidth + 2
        width: 160; height: 170
        border.color: "white"
        radius: 8
        color: "#020f21"
        border.width: 3



        Column {
            id:  pressureCover
            anchors {
                left: parent.left
                leftMargin: 8
                top: parent.top
                topMargin: 5
            }

            QGCLabel{
                    text:           qsTr("   ARM2 STATUS")
                    font.family:    ScreenTools.normalFontFamily
                    font.bold:      true
                    font.pointSize:      12
            }

            Rectangle {
                id: spaceBetween
                width: 10
                height: 10
                color: "transparent"
            }

            QGCLabel {
                text:           qsTr("Pressure:  ")
                font.family:    ScreenTools.normalFontFamily
                font.bold:      true
                font.pointSize:      12
            }

            Row {
                anchors {
                    left: parent.left
                    leftMargin: 30
                }

                FactTextField {
                    showUnits:              false
                    showHelp:               false
                    text:                   _pressueOnBoardARM2.toFixed(2)
                    width:                  70
                    height:                 30
                    horizontalAlignment:    Text.AlignHCenter
                    readOnly:               true
                }

                QGCLabel {
                    text:                   " atm"
                    anchors.verticalCenter: parent.verticalCenter
                    font.bold: true
                    font.pointSize:      12
                }
            }

           Rectangle {
                id: spaceBetween1
                width: 10
                height: 8
                color: "transparent"
            }

            QGCLabel {

               id: rudderStatusText
               text:           qsTr("Temp:        ")
               font.family:    ScreenTools.normalFontFamily
               font.bold:      true
               font.pointSize:      12
            }

            Row {
                anchors {
                    left: parent.left
                    leftMargin: 30
                }

                FactTextField {
                    showUnits:              false
                    showHelp:               false
                    text:                   _tempOnBoardARM2.toFixed(2)
                    width:                  70
                    height:                 30
                    horizontalAlignment:    Text.AlignHCenter
                    readOnly:               true
                }

                QGCLabel {
                    text:                   " oC"
                    anchors.verticalCenter: parent.verticalCenter
                    font.bold: true
                    font.pointSize:      12
                }

            }

        }
    }
}
