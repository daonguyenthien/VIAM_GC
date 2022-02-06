#include "PathController.h"

Q_INVOKABLE PathController::PathController(QObject *parent) : QObject(parent)
{
}

Q_INVOKABLE PathController::~PathController(){

}
Q_INVOKABLE void PathController::generate(QString pathPlanningMode){

    if (pathPlanningMode == "Dubins"){
        qDebug() << "Dubins Path started...";
        PathPlanningDubins* path = new PathPlanningDubins();
        path->clear();
        path->num_Vc = obstacleNumber();
        for(int i = 0;i<path->num_Vc;i++)
        {
            path->Obs_lat[i] = _obstacleAt(i).latitude;
            path->Obs_lng[i] = _obstacleAt(i).longitude;
            path->Obs_R[i] = _obstacleAt(i).radius;
        }
        qDebug()<<"num Vc = "<<path->num_Vc;

        path->los_dubins_main(_waypointList);
        for (int i = 0; i < path->path_latlng.size(); i++)
           _geoPath.addCoordinate(QGeoCoordinate(path->path_latlng[i].x,path->path_latlng[i].y));
        delete path;
    }
    else{
        qDebug() << "Bsplines Path started...";
        PathPlanningBsplines* path = new PathPlanningBsplines();
        path->setBase(10.763915776641571,106.65688877139627);
        path->setConditions(M_PI,1,0,1);
        path->setParameters(4,2,100,0.25);
        if (pathPlanningMode == "Bsplines")
            path->ga_enable = false;
        else
            path->ga_enable = true;
        path->num_Vc = obstacleNumber();
        qDebug()<<"num Vc"<<path->num_Vc;
        for(int i = 0;i<path->num_Vc;i++)
        {
            path->Obs_lat[i] = _obstacleAt(i).getLat();
            path->Obs_lng[i] = _obstacleAt(i).getLng();
            path->Obs_R[i] = _obstacleAt(i).getRadius();
        }
        path->setInputPoints(_waypointList);
        qDebug() << "setup done...";
        path->find_Bspline_path();
        qDebug() << "findBSplinePath done...";
        for (int i = 1; i < 1000; i++)
             _geoPath.addCoordinate(QGeoCoordinate(path->val_lat[i],path->val_lng[i]));

        if (path->ga_enable == true){
            _waypointList.clear();
            for(int i = 0; i<path->numOut; i++){
                appendWaypoint(QGeoCoordinate(path->out_LatLng[i].x,
                                              path->out_LatLng[i].y));
            }
        }
        delete path;
    }
}

Q_INVOKABLE void PathController::appendObstacle(QGeoCoordinate obstacle){
    // We use altitude for obstacle radius!    
    _obstacleList.append({obstacle.latitude(),obstacle.longitude(),obstacle.altitude()});
    qDebug()<<obstacle;
}

Q_INVOKABLE void PathController::deleteObstables (){
    _obstacleList.clear();
}

Q_INVOKABLE void PathController::sortTSP_GA(int max_generation,int max_stall_generation, int POPSIZE,
                                            double cross_prob, double mutate_prob, bool _obs_enabled){
    for (int hi = 0; hi < _waypointList.length(); hi ++){
        qDebug() << hi << ": " << _waypointList[hi];
    }
    TSP_GA* tspGAObject = new TSP_GA();
    tspGAObject->max_generation = max_generation;
    tspGAObject->max_stall_generation = max_stall_generation;
    tspGAObject->pop.POPSIZE = POPSIZE;
    tspGAObject->pop.cross_prob = cross_prob;
    tspGAObject->pop.mutate_prob = mutate_prob;
    tspGAObject->Obs_enable = _obs_enabled;
    tspGAObject->num_Vc = obstacleNumber();
    for(int i = 0;i<obstacleNumber();i++)
    {
        tspGAObject->Obs_lat[i] = _obstacleAt(i).latitude;
        tspGAObject->Obs_lng[i] = _obstacleAt(i).longitude;
        tspGAObject->Obs_R[i] = _obstacleAt(i).radius;
    }
    tspGAObject->setBase(10.763915776641571,06.65688877139627);
    if (_waypointList.length() > 0){
        tspGAObject->TSP_GA_Run(_waypointList);
        deleteGeoPath();
        for (int i = 0; i < tspGAObject->WP_out.length(); i++){
            appendWaypoint(QGeoCoordinate(tspGAObject->WP_out[i].x,
                                          tspGAObject->WP_out[i].y));
        }

        for (int hu = 0; hu < _waypointList.length(); hu ++){
            qDebug() << hu << ":: " << _waypointList[hu];
        }
    }
    else {
        qDebug() << "There ain't no Waypoints";
    }
}

Q_INVOKABLE void PathController::sortTSP_NN(){
    for (int hi = 0; hi < _waypointList.length(); hi ++){
        qDebug() << hi << ": " << _waypointList[hi];
    }
    TSP_NN* tspNNObject = new TSP_NN();
    if (_waypointList.length() > 0){
        tspNNObject->TSP_NN_Run(_waypointList);
        deleteGeoPath();
        for (int i = 0; i < tspNNObject->WP_out.length(); i++){
            appendWaypoint(QGeoCoordinate(tspNNObject->WP_out[i].x,
                                          tspNNObject->WP_out[i].y));
        }

        for (int hu = 0; hu < _waypointList.length(); hu ++){
            qDebug() << hu << ":: " << _waypointList[hu];
        }
    }
    else {
        qDebug() << "There ain't no Waypoints";
    }
}

Q_INVOKABLE QGeoPath PathController::geoPath(){
    return _geoPath;
}

Q_INVOKABLE QList<QGeoCoordinate> PathController::waypointList(){
    return _waypointList;
}

Q_INVOKABLE void PathController::setGeoPath(const QGeoPath &geoPath){
    if (geoPath == _geoPath)
        return;
    _geoPath = geoPath;
    emit geopathChanged();
}

Q_INVOKABLE void PathController::appendWaypoint(QGeoCoordinate waypoint){
    _waypointList.append(waypoint);
}

Q_INVOKABLE void PathController::deleteGeoPath(){
    while (_geoPath.size()){
        _geoPath.removeCoordinate(0);
    }
    _waypointList.clear();
}
