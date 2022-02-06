#ifndef TSP_NN_H
#define TSP_NN_H

#include "TSP_Basic.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <math.h>
#include <list>
#include <numeric>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cstdlib>

using namespace std;

class TSP_NN : public TSP_Basic
{
public:
    double d[100][100] = {};

    void TSP_NN_Run(QList<QGeoCoordinate> waypointList);

private:
    int N_WP;
};

#endif
