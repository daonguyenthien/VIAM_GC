import QtQuick          2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs  1.2
import QtLocation       5.5
import QtPositioning    5.3
import QtQuick.Layouts  1.2
import QtQuick.Window   2.2

import QGroundControl                   1.0
import QGroundControl.FlightMap         1.0
import QGroundControl.ScreenTools       1.0
import QGroundControl.Controls          1.0
import QGroundControl.FactSystem        1.0
import QGroundControl.FactControls      1.0
import QGroundControl.Palette           1.0
import QGroundControl.Controllers       1.0
import QGroundControl.ShapeFileHelper   1.0
import QGroundControl.Airspace          1.0
import QGroundControl.Airmap            1.0

import Custom.PathController            1.0

Item {
    id: _flyViewOverlay

    readonly property var _activeVehicle:       QGroundControl.multiVehicleManager.activeVehicle

    property var    _qgcView:                   qgcView
    property var    _map:                       _flightMap
    property var    _planMasterController:      _qgcView._planMasterController
    property var    _missionController:         _planMasterController.missionController
    property var    _missionItemCount:          _missionController.missionItemCount
    property var    _visualItems:               _missionController.visualItems
    property var    _currentPlanViewIndex:      _missionController.currentPlanViewIndex
    property var    _visualItemsCount:          _visualItems.count

    property var    _missionLineViewComponent
    property var    _tmpMissionStack:           []
    property var    _obstacleObject:            []

    PathController{ id: pathController }

    Button {
        id: buttonPathPlanning
        text: "Path Planning"
        anchors.right: parent.horizontalCenter
        onClicked: {
            console.log("missionItemCount: ", _missionItemCount)
            deleteWaypoint()
            deletePathOnMap()
            if (_missionItemCount !== 0){
                if(_visualItemsCount > 1){
                    for (var i = 0; i < _visualItemsCount; i++)
                        addWaypoint(_visualItems.get(i).coordinate)
                    updatePathOnMap(comboBoxPathPlanning.currentText)
                }
                if(comboBoxPathPlanning.currentText == "GA - Bspline"){
                    while (_visualItemsCount > 1)
                        removeSimpleMissionItem(_visualItemsCount - 1)
                    for (var j = 1; j < pathController.waypointListLength(); j++)
                        insertSimpleMissionItem(pathController.waypointAt(j), _visualItemsCount)
                    deleteWaypoint()
                    for (var i = 0; i < _visualItemsCount; i++)
                        addWaypoint(_visualItems.get(i).coordinate)
                }
            }
            sendToVehicle()
        }
    }

    ComboBox {
        id: comboBoxPathPlanning
        anchors.right: buttonTSP_GA.left
        width: 160
        height: 48
        model: ListModel {
            ListElement { text: "Dubins" }
            ListElement { text: "Bsplines" }
            ListElement { text: "GA - Bspline" }
        }
    }

    Button {
        id: buttonDeletePath
        text: "Delete Path"
        anchors.left: buttonPathPlanning.right
        onClicked: deletePathOnMap()
    }

    Button {
        id: buttonAddCurrentPos
        text: "Add Current Position"
        anchors.left: buttonDeletePath.right
        onClicked: {
            if (_missionItemCount !== 0){
                if(_visualItemsCount > 2){
                    insertSimpleMissionItem(QtPositioning.coordinate(_activeVehicle.latitude, _activeVehicle.longitude), _visualItemsCount)
                    sendToVehicle()
                }
            }
        }
    }

    Button {
        id: buttonAddObstacle
        text: "Add Obstacles"
        anchors.top: buttonDeletePath.bottom
        anchors.left: parent.horizontalCenter
        onClicked: {
            if (_missionItemCount !== 0)
                if(_visualItemsCount > 1)
                    for (var i = 0; i < _visualItemsCount; i++)
                        pathController.appendObstacle(_visualItems.get(i).coordinate)
            setObs()
            buttonAddObstacle.visible = false;
            buttonDeleteObstacle.visible = true;
        }
    }

    Button {
        id: buttonTSP_GA
        text: "TSP GA"
        anchors.right: buttonTSP_NN.left
        anchors.top: buttonDeletePath.bottom
        onClicked: {
            deleteWaypoint()
            deletePathOnMap()
            _tmpMissionStack = []
            if (_missionItemCount !== 0){
                if(_visualItemsCount > 1){
                    for (var i = 0; i < _visualItemsCount; i++)
                        addWaypoint(_visualItems.get(i).coordinate)
                    ga_Parameter.open()
                }
            }
        }
    }

    Button {
        id: buttonTSP_NN
        text: "TSP NN"
        anchors.right: parent.horizontalCenter
        anchors.top: buttonDeletePath.bottom
        onClicked: {
            deleteWaypoint()
            deletePathOnMap()
            _tmpMissionStack = []
            if (_missionItemCount !== 0){
                if(_visualItemsCount > 1){
                    for (var i = 0; i < _visualItemsCount; i++)
                        addWaypoint(_visualItems.get(i).coordinate)
                    performTSP_NN()
                }
            }
            sendToVehicle()
        }
    }

    Button {
        id: buttonDeleteObstacle
        text: "Delete Obstacles"
        anchors.top: buttonDeletePath.bottom
        anchors.left: buttonAddObstacle.right
        visible: false
        onClicked: {
            for (var j = 0; j < pathController.obstacleNumber(); j++)
                _map.removeMapItem(_obstacleObject[j])
            pathController.deleteObstables()
            buttonAddObstacle.visible = true;
            buttonDeleteObstacle.visible = false;
        }
    }

    Dialog {
        id: ga_Parameter
        visible: false
        title: "Choose GA Parameter"
        standardButtons: Dialog.Ok | Dialog.Cancel

        onAccepted: {
            performTSP_GA()
            sendToVehicle()
        }

        onRejected: console.log("Cancel clicked")

        Label {
            id: label_size
            text: "Population size: "
            anchors.top: parent.top
            anchors.left: parent.left
        }

        TextEdit {
            id: value_size
            text: "100"
            anchors.right: parent.right
            anchors.top: parent.top
        }

        Label {
            id: label_generation
            text: "Number of generations:"
            anchors.top: label_size.bottom
        }

        TextEdit {
            id: value_ganeration
            text: "120"
            anchors.right: parent.right
            anchors.top: value_size.bottom
        }

        Label {
            id: label_stall_generation
            text: "Max stall generations:"
            anchors.top: label_generation.bottom
        }

        TextEdit {
            id: value_stall_ganeration
            text: "50"
            anchors.right: parent.right
            anchors.top: label_generation.bottom
        }

        Label {
            id: label_crossover
            anchors.top: label_stall_generation.bottom
            text: "Crossover probability:"
        }

        TextEdit {
            id: value_crossover
            text: "0.9"
            anchors.right: parent.right
            anchors.top: value_stall_ganeration.bottom
        }

        Label {
            id: label_mutation
            anchors.top: label_crossover.bottom
            text: "Mutation probability:"
        }

        TextEdit {
            id: value_mutation
            text: "0.6"
            anchors.right: parent.right
            anchors.top: value_crossover.bottom
        }
    }

    function performTSP_GA(){
        while (_visualItemsCount > 1)
            removeSimpleMissionItem(_visualItemsCount - 1)
        const max_geraration = parseInt(value_ganeration.text)
        const max_stall_generation = parseInt(value_stall_ganeration.text)
        const pop_size = parseInt(value_size.text)
        const cross_prob = parseFloat(value_crossover.text)
        const mutate_prob = parseFloat(value_mutation.text)

        pathController.sortTSP_GA(max_geraration,max_stall_generation,pop_size,cross_prob,mutate_prob,1)
        for (var j = 1; j < pathController.waypointListLength(); j++){
            insertSimpleMissionItem(pathController.waypointAt(j), _visualItemsCount)
        }
    }

    function performTSP_NN(){
        while (_visualItemsCount > 1)
            removeSimpleMissionItem(_visualItemsCount - 1)

        pathController.sortTSP_NN()
        for (var j = 1; j < pathController.waypointListLength(); j++){
            insertSimpleMissionItem(pathController.waypointAt(j), _visualItemsCount)
        }
    }

    function removeSimpleMissionItem(index) {
        _missionController.removeMissionItem(index)
        _missionController.setCurrentPlanViewIndex(_visualItemsCount - 1, true)
    }

    function insertSimpleMissionItem(coordinate, index) {
        var sequenceNumber = _missionController.insertSimpleMissionItem(coordinate, index)
        _missionController.setCurrentPlanViewIndex(sequenceNumber, true)
    }

    function deletePathOnMap(){
        _map.removeMapItem(_missionLineViewComponent)
        _missionLineViewComponent = []
    }

    function updatePathOnMap(pathPlanningMode){
        _missionLineViewComponent = Qt.createQmlObject('import QtLocation 5.3; MapPolyline {}', _map)
        _missionLineViewComponent.line.width = 4.0
        _missionLineViewComponent.line.color = 'white'
        _missionLineViewComponent.path = loadPath(pathPlanningMode)
        _map.addMapItem(_missionLineViewComponent)
    }

    function loadPath(pathPlanningMode){
        var lines = []
        pathController.generate(pathPlanningMode)
        for (var i = 0; i < pathController.geopath.size(); i++){
            lines[i] = pathController.geopath.coordinateAt(i)
        }
        return lines;
    }

    function addWaypoint(waypoint){
        pathController.appendWaypoint(waypoint)
    }

    function deleteWaypoint(){
        pathController.deleteGeoPath()
    }

    function sendToVehicle(){
        _activeVehicle.flightMode = _activeVehicle.pauseFlightMode
        if (_planMasterController.readyForSaveSend)
            _planMasterController.sendToVehicle()
    }

    function setObs(){
        for (var i = 0; i < pathController.obstacleNumber(); i++){
            _obstacleObject[i] = Qt.createQmlObject('import QtLocation 5.3; MapCircle {}', _map)
            _obstacleObject[i].radius = pathController.obsRadiusAt(i)
            _obstacleObject[i].color = 'pink'
            _obstacleObject[i].opacity = 0.5
            _obstacleObject[i].border.width = 3
            _obstacleObject[i].center.latitude = pathController.obsLatAt(i)
            _obstacleObject[i].center.longitude = pathController.obsLngAt(i)
            _map.addMapItem(_obstacleObject[i])
        }
    }
}
