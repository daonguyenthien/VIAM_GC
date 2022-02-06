#ifndef TSP_BASIC_H
#define TSP_BASIC_H

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <math.h>
#include <stdio.h>
#include <vector>
#include <QList>
#include <QGeoCoordinate>
#include <QtDebug>

#include "Custom/PathPlanningDubins.h"

using namespace std;
class TSP_Basic: public PathPlanningDubins
{
public:

    Point waypoints[50];
    Point InitialPoint;
    Point TerminalPoint;
    QVector<Point> WP_out;

    void inputWaypoints(QList<QGeoCoordinate> waypointList);
};

#endif
