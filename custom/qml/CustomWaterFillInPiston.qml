import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs  1.2
import QtQuick.Layouts  1.2

import QGroundControl                       1.0
import QGroundControl.Controls              1.0
import QGroundControl.MultiVehicleManager   1.0
import QGroundControl.ScreenTools           1.0
import QGroundControl.Palette               1.0
import QGroundControl.Controllers           1.0
import QGroundControl.FactSystem            1.0
import QGroundControl.FactControls          1.0

Item {
    id:         waterPistonPage
    anchors.fill:               parent
    z:      5000

    QGCPalette { id: qgcPal; colorGroupEnabled: enabled }

    property var _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle

    property real   _valuePiston: 2 // Math.abs(_activeVehicle.pistonPosition)*0.1
    property real   _maxValuePiston: 5.0
    property real   _minValuePiston: 0.0
    property real   _rangePiston:         Math.abs(_maxValuePiston - _minValuePiston)

    property real   _scaleHeightOfPistonToWater: 28
    property real   _scaleValueOfPistonToLitreWater: 176


    Rectangle{
        id: rectangle
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: ScreenTools.defaultFontPixelWidth * 24
        anchors.bottom:         parent.bottom
        anchors.bottomMargin:    ScreenTools.defaultFontPixelWidth + 2
        width: 180; height: 170
        border.color: "white"
        radius: 8
        color: "#020f21"  //"#801F1F1F"
        border.width: 3

        Column{
            id: labelWater
            anchors.verticalCenter : parent
            width:    parent.width
            spacing:  12

            Row{
                 id: textRow
                 spacing: 8
                 anchors.top: parent.top
                 anchors.topMargin: 5

                 QGCLabel {
                     id:      textWater
                     text: "  WATER FILL IN PISTON"
                     width:  110
                     font.bold: true
                 }
             }

             Row{
                Column{
                    id:  waterBin
                    width: 70
                    height: 120
                    anchors{
                        left: parent.left
                        leftMargin: 15
                        top: parent.top
                        topMargin: 35
                    }
                    Rectangle{
                        id: airSpaceinBin
                        anchors.top: parent.top
                        width: 70
                        height: 120 - _valuePiston*28
                        color: "white"
                    }
                    Rectangle{
                        id: waterSpaceinBin
                        width: 70
                        height: 120 - airSpaceinBin.height
                        anchors.bottom: parent.bottom
                        color: "cyan"
                   }

                }

                Column{
                    id: labelValueWater
                    anchors{
                        left: waterBin.right
                        leftMargin: 10
                        top: parent.top
                        topMargin: 65
                    }
                    width: 70
                    height: 100
                    FactTextField {
                        showUnits:              false
                        showHelp:               false
                        text:                   (_valuePiston*_scaleValueOfPistonToLitreWater).toFixed(1)
                        width:                  70
                        height:                 40
                        horizontalAlignment:        Text.AlignHCenter
                        readOnly:               true
                    }
                    QGCLabel{

                         text:           "    ml"
                         font.family:    ScreenTools.normalFontFamily
                         font.bold:      true
                         font.pointSize:      15
                    }
                 }

      }
        }
    }

}

