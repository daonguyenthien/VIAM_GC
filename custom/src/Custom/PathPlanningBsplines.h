#ifndef PATHPLANNINGBSPLINES_H
#define PATHPLANNINGBSPLINES_H

//#include <QMainWindow>
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
#include <stdbool.h>
//#include "ui_main_window.h"
#include "PathPlanning.h"

using namespace std;


class PathPlanningBsplines: public PathPlanning
{

public:
    PathPlanningBsplines(QObject* parent = nullptr);
    ~PathPlanningBsplines();

  void setInputPoints(QList<QGeoCoordinate> waypointList);
  void setParameters(unsigned _order, unsigned _alpha, int _gen_num, double _max_turn);
  void setConditions(double _start_yaw, double _start_amp, double _end_yaw, double _end_amp);
  void find_Bspline_path();
  inline double* getOutputBSpline_px() {return px;}
  inline double* getOutputBSpline_py() {return py;}
  inline double* getOutputBSpline_u() {return u;}
  int getBSpline_inverted(){return full_num;}
  double getMax_Curvature();

public:
  unsigned order;
  unsigned alpha;
  double start_yaw, start_amp, end_yaw, end_amp;

  QVector<Point> out_LatLng;

    //input waypoint from map lat lng cor
  double inLat[20];
  double inLng[20];

  //wp in xy cor
  double _x[20];
  double _y[20];

  //output Bspline
  double px[30];
  double py[30];
  double u[30];
  //output Vehinh function
  double val_x[1000];
  double val_y[1000];

  double val_lat[1000];
  double val_lng[1000];
  //GA
  bool ga_enable;
//  int index_change ,num_wp_change,type_change, num_wp_total;
  int gen_num, full_num, N;
//  double  max_turn,space;
  int index_change ,num_wp_change,type_change, num_wp_total;
  double  max_turn,space;

  void minor_(double b[100][100], double a[100][100], int i, int n);
  double det(double a[100][100], int n);
  void transpose(double c[100][100], double d[100][100], int n, double det);
  void cofactor(double a[100][100], double d[100][100], int n,
                double determinte);
  void inverse(double a[100][100], double d[100][100], int n, double det);
  int matrix_inversion(float* A, int n, float* AInverse);
  void multi(double A[][100], double B[][2], double C[][2], unsigned int r1,
             unsigned int c1, unsigned int r2, unsigned int c2);

  double Basic_Bspline(double u[], double t0, unsigned order, unsigned n);
  void Fix_DEBOOR(double val[], double _T[], double _px[], double _py[],
                  double t0, unsigned _order, int t_length);
  void Bline_to_invert_heading(double _px[],double _py[],double u[],double _x[], double _y[],
                                            unsigned _order, unsigned _alpha,unsigned n,
                                            double a1, double a2, double L1,double L2);
  void Bline_to_invert(double _px[], double _py[], double u[], double _x[],
                       double _y[], unsigned _order, unsigned _alpha,
                       unsigned n);
  void Vehinh(double val_x[], double val_y[], double u[], double _px[],
              double _py[], double _x[], double _y[], unsigned _order,
              unsigned n, int np);
  void Bspline_Mapgui(double ar_in_lat[], double ar_in_lng[], double ar_obs_lat[], double ar_obs_lng[], double obs_radius[],
                      int num_wp_in, double b_lat, double b_lng,
                      int num_wp_out, unsigned order, unsigned alpha);
  void Bspline_Mapgui_notGA(double ar_in_lat[], double ar_in_lng[], int num_wp_in,
                      double b_lat, double b_lng,
                      int num_wp_out, unsigned order, unsigned alpha);
  void Construct_WP(double New_Set_WP[], double WP_o[],double WP_new[] ,int index,int num_change,int num_wp_total) ;
  void new_fitnesseval(double result[],double P[][30],int Npopu,int num_wp_change);
  void NaturalSelection (double Select_idex[],double Fitvalue[],int n);
  void New_Crossover ( double Child[][30], double Parent[][30],double Mutationrate,double limit1[], double limit2[],
                        int NParent,int NPopu, int B);
  void GA(double x_WP[],double y_WP[],double MutationProb, int Npopu,int Nparent,int gen_num);
  void GA_heading(double x_WP[],double y_WP[],double L[2],double MutationProb, int Npopu,int Nparent,int gen_num);
  void new_fitnesseval_heading(double result[],double P[][30],int Npopu,int num_wp_change);
};

#endif // PATHPLANNINGBSPLINES_H
