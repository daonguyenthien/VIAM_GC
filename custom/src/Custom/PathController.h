#include <QGeoPath>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>
#include <QDebug>
#include <QList>
#include <QGeoCoordinate>
#include <QtDebug>

#include "SimpleMissionItem.h"
#include "QmlObjectListModel.h"
#include "PathPlanningDubins.h"
#include "PathPlanningBsplines.h"
#include "TSP/TSP_GA.h"
#include "TSP/TSP_NN.h"

class PathController : public QObject {

    Q_OBJECT
    Q_PROPERTY(QGeoPath                 geopath         READ geoPath        WRITE setGeoPath NOTIFY geopathChanged)
    Q_PROPERTY(QList<QGeoCoordinate>    waypointlist    READ waypointList)
public:
    Q_INVOKABLE                             PathController(QObject *parent = 0);
    Q_INVOKABLE                             ~PathController();

    Q_INVOKABLE     void                    generate(QString pathPlanningMode);
    Q_INVOKABLE     void                    sortTSP_GA(int max_generation,int max_stall_generation, int POPSIZE,
                                                                 double cross_prob, double mutate_prob, bool _obs_enabled);
    Q_INVOKABLE     void                    sortTSP_NN();

    Q_INVOKABLE     QList<QGeoCoordinate>   waypointList();
    Q_INVOKABLE     QGeoCoordinate          waypointAt(int index){return _waypointList[index];}
    Q_INVOKABLE     int                     waypointListLength(){return _waypointList.length();}

    Q_INVOKABLE     QGeoPath                geoPath();
    Q_INVOKABLE     void                    setGeoPath(const QGeoPath &geoPath);
    Q_INVOKABLE     void                    appendWaypoint(QGeoCoordinate waypoint);
    Q_INVOKABLE     void                    deleteGeoPath();

    struct Obstacle{
        double latitude;
        double longitude;
        double radius;
        double getLat(){return latitude;}
        double getLng(){return longitude;}
        double getRadius(){return radius;}
    };

    QList<Obstacle>                     _obstacleList;
    Obstacle _obstacleAt(int index)     {return _obstacleList.at(index);}

    Q_INVOKABLE void                    appendObstacle  (QGeoCoordinate obstacle);
    Q_INVOKABLE int                     obstacleNumber  (){ return _obstacleList.length(); }
    Q_INVOKABLE double                  obsLatAt(int index){ return _obstacleList.at(index).latitude;}
    Q_INVOKABLE double                  obsLngAt(int index){ return _obstacleList.at(index).longitude;}
    Q_INVOKABLE double                  obsRadiusAt(int index){ return _obstacleList.at(index).radius;}
    Q_INVOKABLE void                    deleteObstables ();

signals:
    Q_INVOKABLE     void                geopathChanged();

private:
    Q_INVOKABLE     QList<QGeoCoordinate>   _waypointList;
    Q_INVOKABLE     QGeoPath                _geoPath;
//    path_generation_dubins *dubinsPath;
};
