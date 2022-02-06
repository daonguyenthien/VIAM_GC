#ifndef PATHPLANNING_H
#define PATHPLANNING_H

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

#define Earth_radius 6371000

using namespace std;
class PathPlanning {
public:
    PathPlanning();
    ~PathPlanning();

    struct Point
    {
      double x;
      double y;
    };

    int numIn;
    int numOut;
    int num_Vc;

    vector <Point> wp_latlng_in;  // input waypoint in latlng
    vector <Point> wp_xy_in;
    vector <Point> wp_xy_out;
    vector <Point> wp_latlng_out; // output waypoints in latlng

    double Obs_R[10];
    double Obs_lat[10];
    double Obs_lng[10];
    double Obs_x[10];
    double Obs_y[10];

    Point base_latlng;       // base GPS
    vector <Point> path_xy;     // Ve hinh
    QVector<Point> path_latlng;

    void ConverttoXY(double lat, double lon, double ref_lat, double ref_lon,
                     double& x, double& y);
    void ConverttoLatLong(double x, double y, double ref_lat, double ref_lon,
                          double& lat, double& lon);
    void setBase(double baseLat, double baseLng);
};

#endif // PATHPLANNING_H
