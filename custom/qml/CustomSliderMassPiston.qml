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
    id: root
    anchors.fill:               parent
    z:      5000
    property var _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle

    ///Property for Mass-Shifter
    property real   _valueMassShifter: _activeVehicle.mass_shifterPosition * 0.1
    property real   _maxValueMassShifter: 20.0
    property real   _minValueMassShifter: -20.0
    property real   _rangeMassShifter:         Math.abs(_maxValueMassShifter - _minValueMassShifter)
    property real   _minIncrementMassShifter:  _rangeMassShifter/50
    property bool   _showDialogStatusMassShifter: false
    property real   _dutyCycle_Mass:   _activeVehicle.mass_shifterDuty
    property real   _rSpeed_Mass:      _activeVehicle.mass_shifterRSpeed
    property real   _current_Mass:     _activeVehicle.mass_shifterCurrent
    property real   _tempOnChip_Mass:  _activeVehicle.mass_shifterTempOnChip
    property real   _tempMotor_Mass:   _activeVehicle.mass_shifterTempAmbient

    ///Property for Piston
    property real   _valuePiston: _activeVehicle.pistonPosition * 0.1
    property real   _maxValuePiston: 8.0
    property real   _minValuePiston: 0.0
    property real   _rangePiston:         Math.abs(_maxValuePiston - _minValuePiston)
    property real   _minIncrementPiston:  _rangePiston/50
    property bool   _showDialogStatusPiston: false
    property real   _dutyCycle_Pis:   _activeVehicle.pistonDuty
    property real   _rSpeed_Pis:      _activeVehicle.pistonRSpeed
    property real   _current_Pis:     _activeVehicle.pistonCurrent
    property real   _tempOnChip_Pis:  _activeVehicle.pistonTempOnChip
    property real   _tempMotor_Pis:   _activeVehicle.pistonTempAmbient

    property int    precision:      2

    function editShowDialog(_visible){
        return !_visible;
    }

    Rectangle{
        id: rectangle
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: ScreenTools.defaultFontPixelWidth - 160
        anchors.bottom:          parent.bottom
        anchors.bottomMargin:    ScreenTools.defaultFontPixelWidth + 2
        width: sliderRow.width + 130
        height: coverColumn.height + 30

        border.color: "white"
        radius: 8
        color: "#801F1F1F"

       Column{
           id: coverColumn
           anchors.centerIn: parent
           width:    parent.width
           spacing:  12
           Row{
                id: sliderRow
                spacing: 8
                x: parent.x + 10
                // Used to find width of value string
                QGCLabel {
                    id:      textMeasure
                    anchors.verticalCenter: parent.verticalCenter
                    text: "MASS-SHIFTER"
                    width:  110
                    font.bold: true
                }

                // Row container for Value: xx.xx +/- (different spacing than parent)
                Row {
                    spacing:                ScreenTools.defaultFontPixelWidth
                    anchors.verticalCenter: parent.verticalCenter

                    QGCLabel {
                        text:                   "Value: "
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    FactTextField {
                        anchors.verticalCenter: parent.verticalCenter
                        showUnits:              false
                        showHelp:               false
                        text:                   _valueMassShifter.toFixed(precision)
                        width:                  50
                        height:                 30
                        horizontalAlignment:        Text.AlignHCenter
                        readOnly:               true
                    }

                    QGCLabel {
                        text:                   "cm"
                        anchors.verticalCenter: parent.verticalCenter
                    }

//                    QGCButton {
//                        height:                 parent.height
//                        width:                  height
//                        text:                   "-"
//                        anchors.verticalCenter: parent.verticalCenter

//                        onClicked:{
//                            _valueMassShifter = Math.max(Math.min(_valueMassShifter - _minIncrementMassShifter, _maxValueMassShifter), _minValueMassShifter)
//                            _activeVehicle.sliderMassShifter = _valueMassShifter
//                        }
//                    }

//                    QGCButton {
//                        height:                 parent.height
//                        width:                  height
//                        text:                   "+"
//                        anchors.verticalCenter: parent.verticalCenter

//                        onClicked:{
//                            _valueMassShifter = Math.max(Math.min(_valueMassShifter + _minIncrementMassShifter, _maxValueMassShifter), _minValueMassShifter)
//                            _activeVehicle.sliderMassShifter = _valueMassShifter
//                        }
//                    }
                }
            }

            // Slider, with minimum and maximum values labeled
            Row {
                width:      parent.width
                spacing:    2

                Rectangle {
                    width: 8; height: 15
                    color: "transparent"
                }

                Rectangle {
                    width: 15; height: 15
                    color: _activeVehicle.mass_shifterLSTail? "green" : "red"
                }

                QGCLabel {
                    id:                  minLabelMassShifter
                    width:               ScreenTools.defaultFontPixelWidth * 4
                    text:                _minValueMassShifter.toFixed(precision)
                    horizontalAlignment: Text.AlignRight

                }

                QGCSlider {
                    id:                 slideMass_Shifter
                    width:              parent.width - minLabelMassShifter.width - maxLabelMassShifter.width - 30 * 2
                    maximumValue:       _maxValueMassShifter.toFixed(precision)
                    minimumValue:       _minValueMassShifter.toFixed(precision)
                    stepSize:           0.1
                    value:              _valueMassShifter
                    tickmarksEnabled:   true
//                    onValueChanged: {
//                        slideMass_Shifter.value = _valueMassShifter
//                    }
                } // Slider

                QGCLabel {
                    id:     maxLabelMassShifter
                    width:  ScreenTools.defaultFontPixelWidth * 5
                    text:   _maxValueMassShifter.toFixed(precision)                    
                }

                Rectangle {
                    width: 15; height: 15
                    color: _activeVehicle.mass_shifterLSHead? "green" : "red"
                }
            }

            Flow{
                id: flowMassShifter
                width:              parent.width
                layoutDirection:    Qt.LeftToRight
                spacing:            8

                Repeater{
                    model: _showDialogStatusMassShifter? 1 : 0
                    Loader {
                        sourceComponent: _showDialogStatusMassShifter ? componentStatusMassShifter : undefined
                    }
                }
            }

            Component{
                id: componentStatusMassShifter
                Rectangle
                {
                    width: parent.parent.width - 10*2
                    height: coverColumnMass.height + 8
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    color: "transparent"

                    Column{
                        id: coverColumnMass
                        width: Math.max(rowDataMass_1.width, rowDataMass_2.width, rowDataMass_3.width)
                        anchors.top: parent.top
                        anchors.topMargin: 6
                        spacing: 6
                        anchors.horizontalCenter: parent.horizontalCenter

                        Row{
                            id: rowDataMass_1
                            spacing: 20

                            QGCLabel {
                                text:           qsTr("Duty Cycle: ") + _dutyCycle_Mass.toFixed(precision) + qsTr(" %")
                                font.family:    ScreenTools.normalFontFamily
                                font.pointSize:      10
                            }

                            QGCLabel {
                                text:           qsTr("rSpeed: ") + _rSpeed_Mass.toFixed(precision) + qsTr(" RPM")
                                font.family:    ScreenTools.normalFontFamily
                                font.pointSize:      10
                            }
                        }

                        Row{
                            id: rowDataMass_2
                            spacing: 20

                            QGCLabel {
                                text:           qsTr("Current: ") + _current_Mass.toFixed(precision) + qsTr(" mA")
                                font.family:    ScreenTools.normalFontFamily
                                font.pointSize:      10
                            }

                            QGCLabel {
                                text:           qsTr("Temp on Chip: ") + _tempOnChip_Mass.toFixed(precision) + qsTr(" oC")
                                font.family:    ScreenTools.normalFontFamily
                                font.pointSize:      10
                            }
                        }

                        Row{
                            id: rowDataMass_3
                            QGCLabel {
                                text:           qsTr("Temp Motor: ") + _tempMotor_Mass.toFixed(precision) + qsTr(" oC")
                                font.family:    ScreenTools.normalFontFamily
                                font.pointSize:      10
                            }
                        }

                        Rectangle{
                            width: parent.width
                            height: 1
                        } // Draw line
                    }
                }
            }

            Row{
                spacing: 8
                x: parent.x + 10
                // Used to find width of value string
                QGCLabel {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "PISTON"
                    width:  110
                    font.bold: true
                }

                // Row container for Value: xx.xx +/- (different spacing than parent)
                Row {
                    spacing:                ScreenTools.defaultFontPixelWidth
                    anchors.verticalCenter: parent.verticalCenter

                    QGCLabel {
                        text:                   "Value: "
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    FactTextField {
                        anchors.verticalCenter: parent.verticalCenter
                        horizontalAlignment: Text.AlignHCenter
                        showUnits:              false
                        showHelp:               false
                        text:                   _valuePiston.toFixed(precision)
                        width:                  50
                        height:                 30
                        readOnly:               true
                    }

                    QGCLabel {
                        text:                   "cm"
                        anchors.verticalCenter: parent.verticalCenter
                    }

//                    QGCButton {
//                        height:                 parent.height
//                        width:                  height
//                        text:                   "-"
//                        anchors.verticalCenter: parent.verticalCenter

//                        onClicked: {
//                            _valuePiston = Math.max(Math.min(_valuePiston - _minIncrementPiston, _maxValuePiston), _minValuePiston)
//                            _activeVehicle.sliderPiston =  _valuePiston
//                        }
//                    }

//                    QGCButton {
//                        height:                 parent.height
//                        width:                  height
//                        text:                   "+"
//                        anchors.verticalCenter: parent.verticalCenter

//                        onClicked: {
//                            _valuePiston = Math.max(Math.min(_valuePiston + _minIncrementPiston, _maxValuePiston), _minValuePiston)
//                            _activeVehicle.sliderPiston =  _valuePiston
//                        }
//                    }
                }
            }

            // Slider, with minimum and maximum values labeled
            Row {
                width:      parent.width
                spacing:    2

                Rectangle {
                    width: 8; height: 15
                    color: "transparent"
                }

                Rectangle {
                    width: 15; height: 15
                    color: _activeVehicle.pistonLSTail? "green" : "red"
                }

                QGCLabel {
                    id:                  minLabelPiston
                    width:               ScreenTools.defaultFontPixelWidth * 4
                    text:                _minValuePiston.toFixed(precision)
                    horizontalAlignment: Text.AlignRight                    
                }

                QGCSlider {
                    id:                 slidePiston
                    width:              parent.width - minLabelPiston.width - maxLabelPiston.width - 30 * 2
                    maximumValue:       _maxValuePiston
                    stepSize:           0.1
                    value:              _valuePiston
                    tickmarksEnabled:   true
//                    onValueChanged: {
//                        slidePiston.value = _valuePiston
//                    }
                } // Slider

                QGCLabel {
                    id:     maxLabelPiston
                    width:  ScreenTools.defaultFontPixelWidth * 5
                    text:   _maxValuePiston.toFixed(precision)
                }

                Rectangle {
                    width: 15; height: 15
                    color: _activeVehicle.pistonLSHead? "green" : "red"
                }
            }

            Flow{
                id: flowPiston
                width:              parent.width
                layoutDirection:    Qt.LeftToRight
                spacing:            8

                Repeater{
                    model: _showDialogStatusPiston? 1 : 0
                    Loader {
                        sourceComponent: _showDialogStatusPiston ? componentStatusPiston : undefined
                    }
                }
            }

            Component{
                id: componentStatusPiston
                Rectangle
                {
                    width: parent.parent.width - 10*2
                    height: coverColumnPitol.height + 8
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    color: "transparent"

                    Column{
                        id: coverColumnPitol
                        width: Math.max(rowDataPis_1.width, rowDataPis_2.width, rowDataPis_3.width)
                        anchors.top: parent.top
                        anchors.topMargin: 6
                        spacing: 6
                        anchors.horizontalCenter: parent.horizontalCenter

                        Row{
                            id: rowDataPis_1
                            spacing: 20

                            QGCLabel {
                                text:           qsTr("Duty Cycle: ") + _dutyCycle_Pis.toFixed(precision) + qsTr(" %")
                                font.family:    ScreenTools.normalFontFamily
                                font.pointSize:      10
                            }

                            QGCLabel {
                                text:           qsTr("rSpeed: ") + _rSpeed_Pis.toFixed(precision) + qsTr(" RPM")
                                font.family:    ScreenTools.normalFontFamily
                                font.pointSize:      10
                            }
                        }

                        Row{
                            id: rowDataPis_2
                            spacing: 20

                            QGCLabel {
                                text:           qsTr("Current: ") + _current_Pis.toFixed(precision) + qsTr(" mA")
                                font.family:    ScreenTools.normalFontFamily
                                font.pointSize:      10
                            }

                            QGCLabel {
                                text:           qsTr("Temp on Chip: ") + _tempOnChip_Pis.toFixed(precision) + qsTr(" oC")
                                font.family:    ScreenTools.normalFontFamily
                                font.pointSize:      10
                            }
                        }

                        Row{
                            id: rowDataPis_3
                            QGCLabel {
                                text:           qsTr("Temp Motor: ") + _tempMotor_Pis.toFixed(precision) + qsTr(" oC")
                                font.family:    ScreenTools.normalFontFamily
                                font.pointSize:      10
                            }
                        }
                    }
                }
            }
        }

        Rectangle{
            id: zoom
            width: 18; height: 18
            anchors.right: parent.right
            anchors.rightMargin: 7
            anchors.top: parent.top
            anchors.topMargin: 7
            color: "transparent"

            Image {
                id: imgCollapseorExpand
                anchors.fill: parent
                source: _showDialogStatusMassShifter? "/res/collapse.png" : "/res/expand.png"
            }

            MouseArea {
                id: mouseClicked
                anchors.fill:       parent
                acceptedButtons:    Qt.LeftButton
                onClicked: {
                    _showDialogStatusMassShifter = root.editShowDialog(_showDialogStatusMassShifter)
                    _showDialogStatusPiston = root.editShowDialog(_showDialogStatusPiston)
                }
            }
        }
    }
}
