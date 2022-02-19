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
    id:         rudderPage
    anchors.fill:               parent
    z:      5000

    property var _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle

    /// Rudder Status
    property real _position:   _activeVehicle.rudderPosition
    property real _speed:      _activeVehicle.rudderSpeed
    property real _temperature:     _activeVehicle.rudderTemperature
    property real _voltage:  _activeVehicle.rudderVoltage
    property real _load:   _activeVehicle.rudderLoad
    property real _minRudderAngle: -60
    property real _maxRudderAngle: 60

    property int precision: 2

    QGCPalette { id: qgcPal; colorGroupEnabled: enabled }

    Rectangle{
        id: rectangle
        anchors.left: parent.left
        anchors.leftMargin: ScreenTools.defaultFontPixelWidth * 2.5
        anchors.bottom:         parent.bottom
        anchors.bottomMargin:    ScreenTools.defaultFontPixelWidth + 180
        width: 270; height: 180
        border.color: "white"
        radius: 12
        color: "#020f21"
        border.width: 3

        Rectangle{
            id: rudderSlider
            anchors.verticalCenter: parent
            anchors.top:   coverRudderMeter.bottom
            anchors.topMargin: ScreenTools.defaultFontPixelWidth - 5
            anchors.left: parent.left
            anchors.leftMargin: ScreenTools.defaultFontPixelWidth  + 5

            Row{
               width: parent.width
               spacing: 2

               Rectangle{
                   width: 8
                   height: 10
                   color: "transparent"
               }

               QGCLabel{
                   id: minLabelRudder
                   width: ScreenTools.defaultFontPixelWidth * 4
                   text: _minRudderAngle.toFixed(precision)
                   horizontalAlignment: Text.AlignRight

               }

               QGCSlider{
                   id:                 slideRudder
                   width:              160
                   maximumValue:       _maxRudderAngle.toFixed(precision)
                   minimumValue:       _minRudderAngle.toFixed(precision)
                   stepSize:           0.1
                   value:              _position
                   tickmarksEnabled:   true
//                    onValueChanged: {
//                        slideRudder.value = _position
//                    }
               }

               QGCLabel{
                   id: maxLabelRudder
                   width: ScreenTools.defaultFontPixelWidth * 4
                   text: _maxRudderAngle.toFixed(precision)
               }
            }

       }

        Rectangle{
            id: coverRudderMeter
            anchors.left: parent.left
            anchors.leftMargin: ScreenTools.defaultFontPixelWidth  + 5
            anchors.top:         parent.top
            anchors.bottomMargin:    ScreenTools.defaultFontPixelWidth + 2
            anchors.topMargin: ScreenTools.defaultFontPixelWidth + 2
            width: 120; height: 130
            color: "transparent"

            RudderMeter {
                id: id_speed
                anchors.fill: parent
                value:  _position.toFixed(precision)
            }
        }

        Rectangle{
            id: coverStatus
            x: parent.width/2 - 30
            anchors{
                top: parent.top
                topMargin: 2
                right: parent.right
                rightMargin: 15
                bottom: parent.bottom
            }

            width: 149; height: 149
            color: "transparent"

            Column{
                spacing: 6
                anchors.top: parent.top
                anchors.topMargin: 10
                anchors.left: parent
                QGCLabel {
                    id: rudderStatusText
                    x: coverStatus.width/2 - 49
                    text:           qsTr("RUDDER STATUS")
                    font.family:    ScreenTools.normalFontFamily
                    font.bold:      true
                    font.pointSize:      12
                }

                Column{
                    spacing: 6

                    QGCLabel {
                        x: coverStatus.width/2 - 42
                        text:           qsTr("  Speed:   ") + _speed.toFixed(precision) + qsTr(" RPM")
                        font.family:    ScreenTools.normalFontFamily
                        font.pointSize:      10
                        font.bold: true
                    }
                    QGCLabel {
                        x: coverStatus.width/2 - 35
                        text:           qsTr("Temp:    ") + _temperature.toFixed(precision) + qsTr(" oC")
                        font.family:    ScreenTools.normalFontFamily
                        font.pointSize:      10
                        font.bold: true
                    }
                    QGCLabel {
                        x: coverStatus.width/2 - 35
                        text:           qsTr("Voltage: ") + _voltage.toFixed(precision) + qsTr(" V")
                        font.family:    ScreenTools.normalFontFamily
                        font.pointSize:      10
                        font.bold: true
                    }
                    QGCLabel {
                        x: coverStatus.width/2 - 42
                        text:           qsTr("  Load:      ") + _load.toFixed(precision) + qsTr(" N.m")
                        font.family:    ScreenTools.normalFontFamily
                        font.pointSize:      10
                        font.bold: true
                    }
                }
            }
        }

    }
}
