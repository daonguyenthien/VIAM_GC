#ifndef PATH_GENERATION_DUBINS_H
#define PATH_GENERATION_DUBINS_H

#include <QMetaObject>
#include <QObject>
#include <QQuickView>
#include <QUrl>
#include <QVariant>
#include <QList>
#include <QQuickItem>
#include <QtMath>
#include "qqml.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QVariantMap>
#include <QQmlContext>
#include <QGeoPath>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <math.h>
#include <stdio.h>
#include <vector>
#include <complex>
#include "PathPlanning.h"

using namespace std;

class PathPlanningDubins: public PathPlanning
{
public:
    PathPlanningDubins(QObject* parent = nullptr);
    ~PathPlanningDubins();

    inline int sign(double x)
    {
      if (x >= 0)
        return 1;
      else
        return -1;
    }

    double R_min = 4.0;
    double path_length = 0;

    Point p_o[50];           // output waypoint Dubins
    int rho[50];

    void clear();
    void los_dubins_main(QList<QGeoCoordinate> waypointList);
    void Dubins_Mapgui();
    void F_Dubins_Path_Planning();
    void setInputPoints(double* input_lat, double* input_lng, int numInputPoints,
                        Point obs_latlng, double* distance_Vc, int numVcpoint,
                        double R_min);
    double scalar_product(Point V1, Point V2);

    void F_Cal_Parametrization_Arc(double& t, double& anpha, Point P1, Point P2,
                                   Point P3, int rho);
    void F_Circle_Circle_Tangent(Point& P1, Point& P2, Point O1,
                                 Point O2, double R1, double R2, int flag, int rho);
    void F_Circle_Through_Waypoint(Point& p, int& rho, Point WP1, Point WP2,
                                   Point WP3, double R);
    void F_Point_Circle_Tangent(Point& pt, Point wp, Point O, double R, int rho);
private:

signals:

public slots:
};
//Q_DECLARE_METATYPE(path_generation_dubins*)

#endif // PATH_GENERATION_DUBINS_H
