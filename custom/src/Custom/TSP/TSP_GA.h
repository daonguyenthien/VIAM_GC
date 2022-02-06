#ifndef TSP_GA_H
#define TSP_GA_H

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

class TSP_GA : public TSP_Basic
{
public:
    struct Population
    {
        int POPSIZE;
        double fitness[500];    //the fitness
        int N_WP;
        int individual[500][500];
        double cross_prob;
        double mutate_prob;
        double eta; //Selectivity coefficient
    }pop, parent, child;
    
    vector <double> bestfit;
    bool Obs_enable;
    int max_generation;
    int max_stall_generation;
    int stall_generation;
    
    void TSP_GA_Run(QList<QGeoCoordinate> waypointList);

private:

    void Init_Population();
    void Select_Linear_Ranking();
    void Crossover();
    void Mutate_Uniform();
    void ShuffleArray(int* arr, int n);
    void Swap(int* number_1, int* number_2);
    void Cal_Fitness();
    int ceilingLinearSearch(double arr[], int n, double x);
};

#endif
