#include "PathPlanningBsplines.h"

PathPlanningBsplines::PathPlanningBsplines(QObject*)
{
}

PathPlanningBsplines::~PathPlanningBsplines() {}

void PathPlanningBsplines::setInputPoints(QList<QGeoCoordinate> waypointList)
{
  numIn = (size_t)waypointList.length();

  //waypoint in

  for(int i = 0; i < numIn; i++){
    this->inLat[i] = waypointList.value(i).latitude();
    this->inLng[i] = waypointList.value(i).longitude();
  }
}

void PathPlanningBsplines::setParameters(unsigned int _order, unsigned int _alpha,
                                int _gen_num, double _max_turn)
{
  this->order = _order;
  this->alpha = _alpha;
  this->gen_num = _gen_num;
  this->max_turn = _max_turn;
}
void PathPlanningBsplines::setConditions(double _start_yaw, double _start_amp, double _end_yaw, double _end_amp)
{
  this->start_yaw = _start_yaw;
  this->start_amp = _start_amp;
  this->end_yaw = _end_yaw;
  this->end_amp = _end_amp;
}

void PathPlanningBsplines::minor_(double b[100][100], double a[100][100], int i, int n)
{
  int j, l, h = 0, k = 0;
  for (l = 1; l < n; l++)
    for (j = 0; j < n; j++)
    {
      if (j == i)
        continue;
      b[h][k] = a[l][j];
      k++;
      if (k == (n - 1))
      {
        h++;
        k = 0;
      }
    }
}

double PathPlanningBsplines::det(double a[100][100], int n)
{
  int i;
  double b[100][100], sum = 0;
  if (n == 1)
    return a[0][0];
  else if (n == 2)
    return (a[0][0] * a[1][1] - a[0][1] * a[1][0]);
  else
    for (i = 0; i < n; i++)
    {
      minor_(b, a, i, n);
      sum = (double)(sum + a[0][i] * pow(-1, i) * det(b, (n - 1)));
    }
  return sum;
}

void PathPlanningBsplines::transpose(double c[100][100], double d[100][100], int n,
                            double det)
{
  int i, j;
  double b[100][100];
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      b[i][j] = c[j][i];
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      d[i][j] = b[i][j] / det;
}

void PathPlanningBsplines::cofactor(double a[100][100], double d[100][100], int n,
                           double determinte)
{
  double b[100][100], c[100][100];
  int l, h, m, k, i, j;
  for (h = 0; h < n; h++)
    for (l = 0; l < n; l++)
    {
      m = 0;
      k = 0;
      for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
          if (i != h && j != l)
          {
            b[m][k] = a[i][j];
            if (k < (n - 2))
              k++;
            else
            {
              k = 0;
              m++;
            }
          }
      c[h][l] = (double)pow(-1, (h + l)) * det(b, (n - 1));
    }
  transpose(c, d, n, determinte);
}

void PathPlanningBsplines::inverse(double a[100][100], double d[100][100], int n,
                          double det)
{
  if (abs(det-0)<0.00001)
    qDebug()<<"\nInverse of Entered Matrix is not possible\n";
  else if (n == 1)
    d[0][0] = 1;
  else
    cofactor(a, d, n, det);
}

int PathPlanningBsplines::matrix_inversion(float* A, int n, float* AInverse)
{
  // A = input matrix (n x n)
  // n = dimension of A
  // AInverse = inverted matrix (n x n)
  // This function inverts a matrix based on the Gauss Jordan method.
  // The function returns 1 on success, 0 on failure.
  int i, j, iPass, imx, icol, irow;
  float det, temp, pivot, factor;
  float* ac = (float*)calloc(n*n, sizeof(float));
  det = 1;
  for (i = 0; i < n; i++)
  {
      for (j = 0; j < n; j++)
      {
          AInverse[n*i+j] = 0;
          ac[n*i+j] = A[n*i+j];
      }
      AInverse[n*i+i] = 1;
  }
  // The current pivot row is iPass.
  // For each pass, first find the maximum element in the pivot column.
  for (iPass = 0; iPass < n; iPass++)
  {
      imx = iPass;
      for (irow = iPass; irow < n; irow++)
      {
          if (fabs(A[n*irow+iPass]) > fabs(A[n*imx+iPass])) imx = irow;
      }

      // Interchange the elements of row iPass and row imx in both A and AInverse.
      if (imx != iPass)
      {
          for (icol = 0; icol < n; icol++)
          {
              temp = AInverse[n*iPass+icol];
              AInverse[n*iPass+icol] = AInverse[n*imx+icol];
              AInverse[n*imx+icol] = temp;
              if (icol >= iPass)
              {
                  temp = A[n*iPass+icol];
                  A[n*iPass+icol] = A[n*imx+icol];
                  A[n*imx+icol] = temp;
              }
          }
      }

      // The current pivot is now A[iPass][iPass].
      // The determinant is the product of the pivot elements.
      pivot = A[n*iPass+iPass];
      det = det * pivot;
      if (abs(det-0.0)<0.000001)
      {
          free(ac);
          return 0;
      }

      for (icol = 0; icol < n; icol++)
      {
          // Normalize the pivot row by dividing by the pivot element.
          AInverse[n*iPass+icol] = AInverse[n*iPass+icol] / pivot;
          if (icol >= iPass) A[n*iPass+icol] = A[n*iPass+icol] / pivot;
      }

      for (irow = 0; irow < n; irow++)
      {
          // Add a multiple of the pivot row to each row.  The multiple factor
          // is chosen so that the element of A on the pivot column is 0.
          if (irow != iPass) factor = A[n*irow+iPass];
          for (icol = 0; icol < n; icol++)
          {
              if (irow != iPass)
              {
                  AInverse[n*irow+icol] -= factor * AInverse[n*iPass+icol];
                  A[n*irow+icol] -= factor * A[n*iPass+icol];
              }
          }
      }
  }

  free(ac);
  return 1;
}

void PathPlanningBsplines::multi(double A[][100], double B[][2], double C[][2],
                        unsigned int r1, unsigned int c1, unsigned int r2,
                        unsigned int c2)
{
  int i, j, k;
  for (i = 0; i < r1; i++)
  {
    for (j = 0; j < c2; j++)
    {
      C[i][j] = 0;
      for (k = 0; k < c1; k++)
      {
        C[i][j] += A[i][k] * B[k][j];
      }
    }
  }
}

double PathPlanningBsplines::Basic_Bspline(double u[], double t0, unsigned int order,
                                  unsigned int n)
{
  double B[order - 1][order - 1];

  unsigned i, j = 0;
  for (i = 0; i < order; i++)
  {
    for (j = i; j < order; j++)
    {
      if (i == 0)
      {
        if (((t0 >= u[n + j - 1]) && t0 < u[n + j]) ||
            ((abs(t0-1)<0.00001) && (abs(t0-u[n + j])<0.00001)))
        {
          B[j][i] = 1;
        }
        else
          B[j][i] = 0;
      }
      else
      {
        if (abs(u[n + j - 1] - u[n + j - i - 1]) < 0.00001)
        {
          B[j][i] = (u[n + j] - t0) / (u[n + j] - u[n + j - i]) * B[j][i - 1];
        }
        else if (u[n + j] - u[n + j - i] == 0)
        {
          B[j][i] = (t0 - u[n + j - i - 1]) /
                    (u[n + j - 1] - u[n + j - i - 1]) * B[j - 1][i - 1];
        }
        else
          B[j][i] = (t0 - u[n + j - i - 1]) /
                        (u[n + j - 1] - u[n + j - i - 1]) * B[j - 1][i - 1] +
                    (u[n + j] - t0) / (u[n + j] - u[n + j - i]) * B[j][i - 1];
      }
    }
  }
  return B[order - 1][order - 1];
}

void PathPlanningBsplines::Fix_DEBOOR(double val[], double _T[], double _px[],
                             double _py[], double t0, unsigned _order,
                             int t_length)
{
  double X[_order][_order];
  double Y[_order][_order];
  int o = 0;

  int k;
  for (int l = 0; l < t_length; l++)
  {
    if (t0 >= _T[l])
    {
      k = l;
    }
  }

  for (int q = k + 1 - _order; q < k + 1; q++)
  {
    X[o][0] = _px[q];
    Y[o][0] = _py[q];
    o++;
  }

  for (int i = 1; i < _order; i++)
  {
    for (int j = i; j < _order; j++)
    {
      double num = t0 - _T[k - _order + j + 1];

      double weight = 0;
      double s = 0;
      if (abs(num-0)<0.00001)
        weight = 0;
      else
      {
        s = _T[k + j - i + 1] - _T[k + 1 - _order + j];
        weight = num / s;
      }
      X[j][i] = (1 - weight) * X[j - 1][i - 1] + weight * X[j][i - 1];
      Y[j][i] = (1 - weight) * Y[j - 1][i - 1] + weight * Y[j][i - 1];
    }
  }

  val[0] = X[_order - 1][_order - 1];
  val[1] = Y[_order - 1][_order - 1];
}
void PathPlanningBsplines::Bline_to_invert_heading(double _px[],double _py[],double u[],double _x[], double _y[],
                                          unsigned _order, unsigned _alpha,unsigned n,
                                          double a1, double a2, double L1,double L2)
{
    double D[n+2][2];
    double N[100][100];

  //	double u[n+_order];
    double k[n-1];
    double t[n];
    double t_new[n+2];
    double sum_k = 0;

    t[0] = 0;
    for (int i = 1; i < n+1; i++) {
      D[i][0] = _x[i-1];
      D[i][1] = _y[i-1];
    }


    for(int i = 0; i < _order; i++) {
      u[i] = 0;
      u[n + i+2] = 1;
    }
    // tim t
    for (int i = 0; i < (n - 1); i++) {
      k[i] =  pow(  sqrt(pow((D[i+1][0] - D[i + 2][0]),2) + pow((D[i+1][1] - D[i + 2][1]), 2)) , 1/_alpha)  ;

      sum_k += k[i];
    }


    for(int i = 1; i < n; i++) {
      double S = 0;
      for(int j = 0; j < i ; j++) {
        S += k[j];
      }
      t[i] = S / sum_k;
    }

    // tim kno()
    for(int i = _order-1; i < n+1; i++) {
      u[i+1] = 0;
      for (int j = i - _order; j < i - 1; j++) {
        u[i+1] += t[j + 1] / (_order - 1);
      }
    }

    for (int i = 1; i < n+1; i++) {
      D[i][0] = _x[i-1];
      D[i][1] = _y[i-1];
    }

    for (int i = 0; i < n; i++) {
      t_new[i+1]=t[i];
    }
    t_new[0]=0;
    t_new[1]=0.001;
    t_new[n]=0.999;
    t_new[n+1]=1;

    float A[(n+2)*(n+2)];
    float AInverse[(n+2)*(n+2)];
    int a;
    for(int i =0; i < n+2; i++) {

      for (int j = 0; j < n+2 ; j++) {

        if ( (i!=1) && (i!=n)) {

          A[(n+2)*i+j]=Basic_Bspline(u, t_new[i], _order,j+1 ) ;


        } else A[(n+2)*i+j]  = 0 ;
      }
    }

    A[(n+2)]  = -1 ;
    A[(n+2)+1]  = 1 ;
    A[(n+2)*n+n]  = -1 ;
    A[(n+2)*n+n+1]  = 1 ;
    double phi1,phi2;

    phi1= a1*M_PI/180;
    phi2= a2*M_PI/180;
    L1=L1*20;
    L2=L2*20;

    D[0][0]= D[1][0];
    D[0][1]= D[1][1];
    D[n+1][0]=D[n][0];
    D[n+1][1]=D[n][1];
    D[1][0] = u[_order]/(_order-1)* L1*cos(phi1);
    D[1][1] = u[_order]/(_order-1)*L1*sin(phi1);
    D[n][0]= (1-u[n+2-1])/(_order-1)* L2*cos(phi2) ;
    D[n][1]= (1-u[n+2-1])/(_order-1)* L2*sin(phi2) ;

    double N_inv[100][100];
    double P[100][2];
  //printf(N);

    a = matrix_inversion(A,  n+2, AInverse);

    for(int i =0; i < n+2; i++) {
      for (int j = 0; j < n+2 ; j++) {
        N_inv[i][j] =	AInverse[(n+2)*i+j] ;
      }
    }
    /*
    for(int i =0; i < n+2; i++) {

      for (int j = 0; j < n+2 ; j++) {

        cout<<"\t "<<N_inv[i][j];

      }
      cout<<"---"<<endl;
    } */

    multi(N_inv,D,P,n+2,n+2,n+2,2);

    for (int i=0; i<n+2; i++) {
      _px[i] = P[i][0] ;
      _py[i] = P[i][1] ;
    //	cout<<_px[i]<<","<<_py[i]<<endl;
    }
}


void PathPlanningBsplines::Bline_to_invert(double _px[], double _py[], double u[],
                                  double _x[], double _y[], unsigned _order,
                                  unsigned _alpha, unsigned n)
{
//  std::cout<<"Bline_to_invert"<<std::endl;
//  std::cout<<"n="<<n<<std::endl;
//  std::cout<<"_order="<<_order<<std::endl;
//  std::cout<<"_alpha="<<_alpha<<std::endl;
    double D[n][2];
    double N[100][100];
  //	double u[n+_order];
    double k[n-1];
    double t[n];
    double sum_k = 0;

    t[0] = 0;
    for (int i = 0; i < n; i++) {
      D[i][0] = _x[i];
      D[i][1] = _y[i];
    }
    for(int i = 0; i < _order; i++) {
      u[i] = 0;
      u[n + i] = 1;
    }
    // tim t
    for (int i = 0; i < (n - 1); i++) {
      k[i] =  pow(  sqrt(pow((D[i][0] - D[i + 1][0]),2) + pow((D[i][1] - D[i + 1][1]), 2)) , 1)  ;
      sum_k += k[i];
    }


    for(int i = 1; i < n; i++) {
      double S = 0;
      for(int j = 0; j < i ; j++) {
        S += k[j];
      }
      t[i] = S / sum_k;
    }

    // tim kno()
    for(int i = _order; i < n; i++) {
      u[i] = 0;
      for (int j = i - _order; j < i - 1; j++) {
        u[i] += t[j + 1] / (_order - 1);
      }
    }


    // tim ma tran N

    float A[n*n];
  float AInverse[n*n];
  int a;


    for(int i =0; i < n; i++) {

      for (int j = 0; j < n ; j++) {
        N[i][j] =	Basic_Bspline(u, t[i], _order,j+1 ) ;
        A[n*i+j] = 	N[i][j] ;
      }
    }
    double N_inv[100][100];
    double P[100][2];
    //printf(N);
  //	double deter = (double)det(N,n);
  //	std::cout<<"deter="<<deter<<std::endl;
//    std::cout<<"n="<<n<<std::endl;

  //	inverse(N,N_inv,n,deter);



//    std::cout<<"new_way:"<<endl;

    a = matrix_inversion(A,  n, AInverse);


    for(int i =0; i < n; i++) {
      for (int j = 0; j < n ; j++) {
        N_inv[i][j] =	AInverse[n*i+j] ;
      }
    }



//    std::cout<<"multi:"<<endl;
    multi(N_inv,D,P,n,n,n,2);

    for (int i=0; i<n; i++) {
      _px[i] = P[i][0] ;
      _py[i] = P[i][1] ;
    }

}

void PathPlanningBsplines::Vehinh(double val_x[], double val_y[], double u[],
                         double _px[], double _py[], double _x[], double _y[],
                         unsigned _order, unsigned n, int np)
{
  double t0;
  double val[2];
  for (int i = 0; i <= np; i++)
  {
    t0 = (double)i / np;
    Fix_DEBOOR(val, u, _px, _py, t0, _order, n);
    val_x[i] = val[0];
    val_y[i] = val[1];
    ConverttoLatLong(val_x[i], val_y[i], base_latlng.x, base_latlng.y, val_lat[i], val_lng[i]);
  }
}

void PathPlanningBsplines::find_Bspline_path()
{
    qDebug()<<Obs_lat[0];
  if (!ga_enable){
    numOut = numIn;
    Bspline_Mapgui_notGA(inLat, inLng, numIn, base_latlng.x, base_latlng.y, numIn, order, alpha);
  }
  else{
    numOut = 2*numIn-1;
    Bspline_Mapgui(inLat, inLng, Obs_lat, Obs_lng, Obs_R, numIn, base_latlng.x, base_latlng.y, numOut, order, alpha);
  }
}

void PathPlanningBsplines::Bspline_Mapgui(double ar_in_lat[], double ar_in_lng[],
                                          double ar_obs_lat[], double ar_obs_lng[], double obs_radius[], int num_wp_in,
                                          double b_lat, double b_lng, int num_wp_out, unsigned order, unsigned alpha)
{
  // doi dau vao
//  double _x[num_wp_in];
//  double _y[num_wp_in];
  double xout;
  double yout;

for (int i = 0; i < num_Vc; i++)
    ConverttoXY(Obs_lat[i], Obs_lng[i], base_latlng.x, base_latlng.y,
                Obs_x[i], Obs_y[i]);

  for (int i = 0; i < num_wp_in; i++)
  {
    ConverttoXY(ar_in_lat[i], ar_in_lng[i], b_lat, b_lng, xout, yout);
    _x[i] = xout;
    _y[i] = yout;
  }
  for (int i = 0; i < num_Vc; i++)
  {
    ConverttoXY(ar_obs_lat[i], ar_obs_lng[i], b_lat, b_lng, xout, yout);
    Obs_x[i] = xout;
    Obs_y[i] = yout;
    // cout<<"Obs_x="<<Obs_x[i]<<endl;
    // cout<<"Obs_y="<<Obs_y[i]<<endl;
    // cout<<"---------------"<<endl;
    qDebug()<<2<<ar_obs_lat[i];
  }
/****************************************/
//  gen_num =200;//
  num_wp_total = num_wp_in;
  numIn = N = num_wp_in;
  num_wp_change = N-1;
  full_num = num_wp_total+num_wp_change+2;
  type_change = 1;

  index_change = 0 ;

  space = 0.005;
//  max_turn= 0.5;//

  double MutationProb = 0.2;
  // Xac suat dot bien
  int Npopu=100;
  // So luong ca the cua quan the
  int Nparent=50;
  // So ca the dem lai
  // Caculation
  int n = 2*num_wp_change;
  double x_o[n], dt[n],limit1[n],limit2[n];

  /************************************************/

//  double Bspline_px[full_num] , Bspline_py[full_num] , Bspline_u[full_num + order];
//  int wp=1000;
//  double X[wp], Y[wp], Bspline_psid[wp];
  double x_WP[full_num];
  double y_WP[full_num];


  double L[2],L1,L2;
//  GA(x_WP,y_WP,MutationProb,Npopu, Nparent, gen_num);
  GA_heading(x_WP,y_WP,L,MutationProb, Npopu,Nparent,gen_num);

//  cout<<"x="<<endl;
//  for(int i=0;i<num_wp_in;i++)
//    cout<<_x[i]<<endl;
//  cout<<"y="<<endl;
//  for(int i=0;i<num_wp_in;i++)
//    cout<<_y[i]<<endl;

//  cout<<"x_WP="<<endl;
//  for(int i=0;i<full_num;i++)
//    cout<<x_WP[i]<<endl;

//  cout<<"y_WP="<<endl;
//  for(int i=0;i<full_num;i++)
//    cout<<y_WP[i]<<endl;
  //num_wp_in: number waypoint, 2: hai   dieu kien dau cuoi
  // Tim bspline
//  double _px[full_num+2];
//  double _py[full_num+2];
//  double u[full_num+2 + order];

//  cout<<"full_num="<<full_num<<endl;
//  Bline_to_invert_heading(_px, _py, u, x_WP, y_WP, order, alpha, full_num, start_yaw, end_yaw, start_amp, end_amp);
  L1=L[0];
  L2=L[1];
//  qDebug()<<"L1="<<L1<<endl;
//  qDebug()<<"L2="<<L2<<endl;
  Bline_to_invert_heading(px,py,u,x_WP,y_WP,order,alpha,full_num-2,start_yaw,end_yaw,L1,L2);

//  Bline_to_invert(px, py, u, x_WP, y_WP, order, alpha, full_num);

//  cout<<"Bspline_x="<<endl;
//  for(int i=0;i<full_num;i++)
//    cout<<_px[i]<<endl;

//  cout<<"Bspline_y="<<endl;
//  for(int i=0;i<full_num;i++)
//    cout<<_py[i]<<endl;
  Vehinh(val_x, val_y, u, px, py, _x, _y, order, full_num, 1000);
  // doi lai dau ra
  double tmp_lat, tmp_lng;
  Point tmp_out;
  for (int i = 0; i < full_num - 2; i++)
  {
//    qDebug()<< x_WP[i] << " " << y_WP[i];
    ConverttoLatLong(x_WP[i], y_WP[i], b_lat, b_lng, tmp_lat, tmp_lng);
    tmp_out.x = tmp_lat;
    tmp_out.y = tmp_lng;
    out_LatLng.append(tmp_out);
  }
}

void PathPlanningBsplines::Bspline_Mapgui_notGA(double ar_in_lat[], double ar_in_lng[], int num_wp_in,
                                                double b_lat, double b_lng,
                                                int num_wp_out, unsigned order, unsigned alpha)
{
/****************************************/
  full_num = num_wp_in;
  double xout, yout;
  for (int i = 0; i < num_wp_in; i++)
  {
    ConverttoXY(ar_in_lat[i], ar_in_lng[i], b_lat, b_lng, xout, yout);
    _x[i] = xout;
    _y[i] = yout;
  }

//  Bline_to_invert_heading(_px, _py, u, x_WP, y_WP, order, alpha, full_num, start_yaw, end_yaw, start_amp, end_amp);
  Bline_to_invert(px, py, u, _x, _y, order, alpha, full_num);

  Vehinh(val_x, val_y, u, px, py, _x, _y, order, full_num, 1000);
  // doi lai dau ra
  double tmp_lat, tmp_lng;
  Point tmp_out;

  for (int i = 0; i < num_wp_out; i++)
  {
    ConverttoLatLong(_x[i], _y[i], b_lat, b_lng, tmp_lat, tmp_lng);
    tmp_out.x = tmp_lat;
    tmp_out.y = tmp_lng;
    out_LatLng.append(tmp_out);
  }
}

void PathPlanningBsplines::Construct_WP(double New_Set_WP[], double WP_o[],double WP_new[] ,int index,int num_change,int num_wp_total)
{
  // Example : Construct_WP([1 2 3 4 5 6],[8 4 7 6],2,4)

    for (int i=0; i<index; i++) {

      New_Set_WP[i]= WP_o[i];
    }
    for (int i=0; i<num_change; i++) {

      New_Set_WP[2*i+index]= WP_o[index+i];
      New_Set_WP[2*i+index+1]=WP_new[i];
    }

    for (int i=0; i<-num_change+num_wp_total-index; i++) {
      New_Set_WP[i+index+2*num_change ]= WP_o[i+ index+num_change];
    }
    New_Set_WP = WP_o;
}

void PathPlanningBsplines::new_fitnesseval(double result[],double P[][30],int Npopu,int num_wp_change)
{
  double  g;
  int wp = 1000;
  double temp_fit[Npopu];
  double temp_cstr[Npopu] ;
  double temp_mat[Npopu];
  int ind_tmp_mat =0;
  double min_S = 1000, max_S_val =0, S = 0;
  double temp_S[Npopu];
  double temp_x[num_wp_change];
  double temp_y[num_wp_change];
  int full_num = num_wp_total+num_wp_change;
  double x_WP[full_num];
  double y_WP[full_num];
  double Bspline_px[full_num] , Bspline_py[full_num] , Bspline_u[full_num + order];
  double X[wp], Y[wp], Bspline_psid[wp];

  for (int u = 0; u<Npopu; u++) {
    for (int j=0; j<num_wp_change; j++) {
      temp_x[j] = P[u][j];
      temp_y[j] = P[u][j+num_wp_change];
    }
    Construct_WP(x_WP, _x,temp_x ,index_change,num_wp_change,num_wp_total);
    Construct_WP(y_WP, _y,temp_y ,index_change,num_wp_change,num_wp_total);

    Bline_to_invert(Bspline_px,Bspline_py,Bspline_u,x_WP,y_WP,order,alpha,full_num);
    Vehinh(X, Y,Bspline_u,Bspline_px,Bspline_py,x_WP,y_WP,order,full_num, wp);

    double dx[wp], dy[wp],d2x_[wp],d2y_[wp],k[wp];


    for (int i=1; i<wp-1; i++) {
      dy[i]=Y[i+1]-Y[i];
      dx[i]=X[i+1]-X[i];
    }
    for (int i=0; i<wp-2; i++) {
      d2y_[i]=dy[i+1]-dy[i];
      d2x_[i]=dx[i+1]-dx[i];
    }
    d2x_[wp-2] = 0;
    d2y_[wp-2] = 0;

    for (int i=0; i<wp-1; i++) {
      k[i]=(dx[i]*d2y_[i]-dy[i]*d2x_[i])/pow( (pow(dx[i],2)+pow(dy[i],2)),1.5);
    }

    double tmp_mm = k[0];

    //int ind_mm = 0;
    for (int p =1; p<wp-1; p++) {
      if ( abs(k[p])>tmp_mm) {
        tmp_mm = abs(k[p]);
        //ind_mm= i;
      }
    }

    g = max_turn- tmp_mm;
    temp_cstr[u] =g;

    if (g<0 )
      temp_fit[u] = 1;
    else {
      S=0;
      for (int j= 2; j<wp-1; j++)
        S=S + sqrt(  pow(X[j]-X[j-1],2) + pow(Y[j]-Y[j-1],2) );
      temp_S[u]=S;
      result[u]=S;
      temp_mat[ind_tmp_mat]= S;
      ind_tmp_mat ++;
    }
  }

  for (int p =0; p<Npopu-1; p++) {
    if ( abs(temp_S[p])>max_S_val) {
      max_S_val = abs(temp_S[p]);
      //ind_mm= i;
    }
  }
/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/
  if (abs(max_S_val-0)<0.000001) //toan infeasible
    max_S_val = 2;
  else {
    for (int p =0; p<ind_tmp_mat; p++) {
      if ( abs(temp_mat[p])<min_S) {
        min_S = abs(temp_mat[p]);
        //ind_mm= i;
      }

    }
  }

  for (int u = 0; u<Npopu; u++) {
    if (abs(temp_fit[u]-1)<0.00001 ) { //Neu la ca the infeasible
    //	cout<<"toan inf 1 ";
      if (abs(max_S_val-2)>0.00001 ) { // co them feasible
        result[u]= max_S_val -  temp_cstr[u]/(0.5*max_turn);
      //	cout<<"toan inf 2 ";
        if (result[u] > 2+ max_S_val)
          result[u] = 2 + max_S_val;

      } else { //toan infeasible
        result[u]= max_S_val -  2*temp_cstr[u];
      //	cout<<"toan inf 3";
        if (result[u] > 1000)
          result[u] = 1000;

      }

    }

  }
}

void PathPlanningBsplines::NaturalSelection (double Select_idex[],double Fitvalue[],int n)
{
  int index;
  double sumi,randnum;
  for (int i=0; i<n; i++) {
    randnum=(float)rand()/RAND_MAX;
  //	cout<<"randnum:"<<randnum<<endl;
    sumi=0;
    index=-1;
    while (sumi<randnum) {
      index=index+1;
      sumi=sumi+Fitvalue[index];

    }
    Select_idex[i]=index;
  //	cout<<"index:"<<index<<endl;
  }
}

void PathPlanningBsplines::New_Crossover( double Child[][30], double Parent[][30],double Mutationrate,double limit1[], double limit2[],
                      int NParent,int NPopu, int B)
{
  int mu = NPopu-NParent-1;
  int fixed,temp1,tmp;
  double chi;
  for (int i=0; i<NParent; i++) {
  //	cout<<"B:"<<B<<endl;
    fixed = (float)rand()*mu/RAND_MAX;
  //	cout<<"fixed:"<<fixed<<endl;
    for (int k = 0; k<mu; k++) {
      for (int j = 0; j<B; j++) {
        chi       = (float)rand()/RAND_MAX;
        //tmp       = randsample(1:mu,1);
        tmp=(float)rand()*mu/RAND_MAX;
        Child[i][j]  = Parent[fixed][j]  + chi*(Parent[tmp][j]  - Parent[fixed][j]);
        //cout<<"Child[i][j]"<<Child[i][j]<<endl;
      }
    }

    //mutation

    if ((float)rand()/RAND_MAX < Mutationrate) {

      temp1=(float)rand()*B/RAND_MAX;
    //	cout<<"temp1"<<temp1<<endl;
      Child[i][temp1]= limit1[temp1]+(float)rand()*(limit2[temp1]-limit1[temp1])/RAND_MAX;
    }

  }
}

void PathPlanningBsplines::GA(double x_WP[],double y_WP[],double MutationProb, int Npopu,int Nparent,int gen_num)
{
  int n = 2*num_wp_change;
  double x_o[n], dt[n],limit1[n],limit2[n];

  for(int i=0; i<num_wp_change; i++) {

    x_o[i] =( _x[i] + _x[i+1] )/2.0;

    x_o[i+num_wp_change] =( _y[i] + _y[i+1])/2.0;
    dt[i] = sqrt(  pow( (_x[i] - _x[i+1] ),2) + pow(( _y[i] - _y[i+1] ),2) )*3.0/8.0;
    dt[i+num_wp_change]=dt[i];
    limit1[i]=x_o[i]-dt[i];
    limit2[i]=x_o[i]+dt[i];
    limit1[i+num_wp_change]=x_o[i+num_wp_change]-dt[i+num_wp_change];
    limit2[i+num_wp_change]=x_o[i+num_wp_change]+dt[i+num_wp_change];
//    cout<<limit2[i]<<endl;
  }

  // khoi tao quan the
  double Popu[Npopu][30];
  double Result[n];
  double Fitvalue[Npopu];
  double max_fit_Egen[gen_num];
  double max_curve_Egen[gen_num];
  srand((int) time(0));
  for (int i=0; i<n; i++) {
    for (int j=0; j<Npopu; j++) {
      Popu[j][i]=  limit1[i]+ (float)rand()/RAND_MAX*(limit2[i]-limit1[i]);
    }
  }

  double tmp_mm,tmp_mm2, tmp_sum =0;
  int ind_mm;


  for(int u = 0; u<gen_num; u++) {
    qDebug()<<"Gen:"<<u<<endl;
    new_fitnesseval(Fitvalue,Popu,Npopu,num_wp_change);
    tmp_mm=1000;
    tmp_mm2=0;
    tmp_sum=0;
    for (int i =0; i<Npopu; i++) {
      if ( Fitvalue[i]<tmp_mm) {
        tmp_mm = Fitvalue[i];
        ind_mm= i;
      }
    }
  //	cout<<"1"<<endl;
    //[a,b]=min(Fitvalue);

    for (int i =0; i<Npopu; i++) {
      if ( Fitvalue[i]>tmp_mm2) {
        tmp_mm2 = Fitvalue[i];

      }

    }
/*bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb*/

    if (abs(tmp_mm-tmp_mm2)<0.00001)

      for (int i =0; i<Npopu; i++) {
        Fitvalue[i] = 1.0/Npopu ;
      }

    else {

      for (int i =0; i<Npopu; i++) {
        tmp_sum+=tmp_mm2 - Fitvalue[i];
      }
      for (int i =0; i<Npopu; i++) {
        Fitvalue[i] = (tmp_mm2 - Fitvalue[i] )/tmp_sum;
      }
    }
//    for (int i =0; i<Npopu; i++) {
//    	cout<<"Fitvalue:"<<Fitvalue[i]<<endl;
//    }

  //	cout<<"3"<<endl;
    double Child[Nparent][30], Parent[Npopu-Nparent-1][30],Select_idex[Npopu-Nparent-1],Mutationrate ;
    int tempp;

  //	cout<<"4"<<endl;
    NaturalSelection (Select_idex,Fitvalue, Npopu-Nparent-1);
    for (int i =0; i<Npopu-Nparent-1; i++) {

      for (int j =0; j<n; j++) {
        tempp= (int)Select_idex[i];
        Parent[i][j]= Popu[tempp][j];
      }
    }
    New_Crossover ( Child, Parent,MutationProb,limit1,  limit2,Nparent,Npopu, n);



    for (int j =0; j<n; j++)
      Result[j]=Popu[ind_mm][j];

    for (int i =0; i<Npopu-Nparent-1; i++) {
      for (int j =0; j<n; j++)
        Popu[i][j]=Parent[i][j];
    }
    for (int j =0; j<n; j++)
      Popu[Npopu-Nparent-1][j]=Result[j];
    for (int i =0; i<Nparent; i++) {
      for (int j =0; j<n; j++)
        Popu[i+Npopu-Nparent][j]=Child[i][j];
    }

//Popu=[SelectedIndi; Popu(b,:); NewGen];
  }
  double temp_x[num_wp_change];
  double temp_y[num_wp_change];
//  int full_num = num_wp_total+num_wp_change;




  for (int j=0; j<num_wp_change; j++) {
    temp_x[j] = Result[j];
    temp_y[j] =  Result[j+num_wp_change];

  }
  qDebug()<<"tmp_mm:"<<tmp_mm<<endl;
  Construct_WP(x_WP, _x,temp_x ,index_change,num_wp_change,num_wp_total);
  Construct_WP(y_WP, _y,temp_y ,index_change,num_wp_change,num_wp_total);

//  cout<<"xWP:"<<endl;
//  for (int j=0; j<num_wp_total+num_wp_change; j++) {
//    cout<<x_WP[j]<<";"<<endl;
//  }
//  cout<<"yWP:"<<endl;
//  for (int j=0; j<num_wp_total+num_wp_change; j++) {
//    cout<<y_WP[j]<<";"<<endl;
//  }

}

void PathPlanningBsplines::GA_heading(double x_WP[],double y_WP[],double L[2],double MutationProb, int Npopu,int Nparent,int gen_num) {

  int n = 2*num_wp_change +2; //cong them 2 phan tu --------------------------change------------------
  double x_o[n], dt[n],limit1[n],limit2[n];

  for(int i=0; i<num_wp_change; i++) {

    x_o[i] =( _x[i] + _x[i+1] )/2.0;

    x_o[i+num_wp_change] =( _y[i] + _y[i+1])/2.0;
    dt[i] = sqrt(  pow( (_x[i] - _x[i+1] ),2) + pow(( _y[i] - _y[i+1] ),2) )*3.0/8.0;
    dt[i+num_wp_change]=dt[i];
    limit1[i]=x_o[i]-dt[i];
    limit2[i]=x_o[i]+dt[i];
    limit1[i+num_wp_change]=x_o[i+num_wp_change]-dt[i+num_wp_change];
    limit2[i+num_wp_change]=x_o[i+num_wp_change]+dt[i+num_wp_change];
    qDebug()<<"lim2"<<limit2[i]<<endl;
  }
  //---------------------------change------------------
    limit1[2*num_wp_change]= 8/3/20*dt[0];
    limit2[2*num_wp_change]=dt[0];
    limit1[2*num_wp_change+1]=8/3/20*dt[num_wp_change-1];
    limit2[2*num_wp_change+1]=dt[num_wp_change-1];

  // khoi tao quan the
  double Popu[Npopu][30];
  double Result[n];
  double Best_fit[1][30];//-------
  double Fitvalue[Npopu];
  double max_fit_Egen[gen_num];
  double max_curve_Egen[gen_num];
  srand((int) time(0));
  for (int i=0; i<n; i++) {
    for (int j=0; j<Npopu; j++) {
      Popu[j][i]=  limit1[i]+ (float)rand()/RAND_MAX*(limit2[i]-limit1[i]);
    }
  }

  double tmp_mm,tmp_mm2, tmp_sum =0;
  int ind_mm;


  for(int u = 0; u<gen_num; u++) {
    qDebug()<<"Gen:"<<u<<endl;
    new_fitnesseval_heading(Fitvalue,Popu,Npopu,num_wp_change);
    tmp_mm=1000;
    tmp_mm2=0;
    tmp_sum=0;
    for (int i =0; i<Npopu; i++) {
      if ( Fitvalue[i]<tmp_mm) {
        tmp_mm = Fitvalue[i];
        ind_mm= i;
      }
    }
    qDebug()<<"New:"<<tmp_mm<<endl;
    qDebug()<<"Old:"<<Fitvalue[79]<<endl;
    qDebug()<<ind_mm<<endl;
    //	cout<<"1"<<endl;
    //[a,b]=min(Fitvalue);

    for (int i =0; i<Npopu; i++) {
      if ( Fitvalue[i]>tmp_mm2) {
        tmp_mm2 = Fitvalue[i];

      }
    }

    if (tmp_mm==tmp_mm2)

      for (int i =0; i<Npopu; i++) {
        Fitvalue[i] = 1.0/Npopu ;
      }
    else {

      for (int i =0; i<Npopu; i++) {
        tmp_sum+=tmp_mm2 - Fitvalue[i];
      }
      for (int i =0; i<Npopu; i++) {
        Fitvalue[i] = (tmp_mm2 - Fitvalue[i] )/tmp_sum;
      }
    }
    for (int i =0; i<Npopu; i++) {
      //	cout<<"Fitvalue:"<<Fitvalue[i]<<endl;
    }

    //	cout<<"3"<<endl;

    double Child[Nparent][30], Parent[Npopu-Nparent-1][30],Select_idex[Npopu-Nparent-1],Mutationrate ;
    int tempp;

    //	cout<<"4"<<endl;
    NaturalSelection (Select_idex,Fitvalue, Npopu-Nparent-1);
    for (int i =0; i<Npopu-Nparent-1; i++) {

      for (int j =0; j<n; j++) {
        tempp= Select_idex[i];
        Parent[i][j]= Popu[tempp][j];
      }
    }

    New_Crossover ( Child, Parent,MutationProb,limit1,  limit2,Nparent,Npopu, n);



    for (int j =0; j<n; j++) {
      Result[j]=Popu[ind_mm][j];
      Best_fit[0][j]=Popu[ind_mm][j];}

    for (int i =0; i<Npopu-Nparent-1; i++) {
      for (int j =0; j<n; j++)
        Popu[i][j]=Parent[i][j];
    }
    for (int j =0; j<n; j++)
      Popu[Npopu-Nparent-1][j]=Result[j];
    for (int i =0; i<Nparent; i++) {
      for (int j =0; j<n; j++)
        Popu[i+Npopu-Nparent][j]=Child[i][j];
    }

//Popu=[SelectedIndi; Popu(b,:); NewGen];
  }


  double temp_x[num_wp_change];
  double temp_y[num_wp_change];
  int full_num = num_wp_total+num_wp_change;


  new_fitnesseval_heading(Fitvalue,Best_fit,1,num_wp_change);
  qDebug()<<"Fit_value:"<<Fitvalue;
  for (int j=0; j<num_wp_change; j++) {
    temp_x[j] = Result[j];
    temp_y[j] =  Result[j+num_wp_change];

  }
  qDebug()<<"tmp_mm:"<<tmp_mm<<endl;
  Construct_WP(x_WP, _x,temp_x ,index_change,num_wp_change,num_wp_total);
  Construct_WP(y_WP, _y,temp_y ,index_change,num_wp_change,num_wp_total);

  L[0]=Result[2*num_wp_change];
  L[1]=Result[2*num_wp_change+1];
  // cout<<"xWP:"<<endl;
  // for (int j=0; j<num_wp_total+num_wp_change; j++) {
  //   cout<<x_WP[j]<<";"<<endl;
  // }
  // cout<<"yWP:"<<endl;
  // for (int j=0; j<num_wp_total+num_wp_change; j++) {
  //   cout<<y_WP[j]<<";"<<endl;
  // }
  // cout<<"L[0]"<<L[0]<<endl;
  // cout<<"L[1]"<<L[1]<<endl;

}

void PathPlanningBsplines::new_fitnesseval_heading(double result[],double P[][30],int Npopu,int num_wp_change) {
  int wp= 1000;
  double g,g1,g2;
  double temp_fit[Npopu];
  double temp_cstr[Npopu];
  double temp_mat[Npopu] ;
  int ind_tmp_mat =0;
  double min_S = 100000, max_S_val =0, S = 0;
  double temp_S[Npopu];
  double temp_x[num_wp_change];
  double temp_y[num_wp_change];
  int full_num = num_wp_total+num_wp_change+2;
  double x_WP[full_num];
  double y_WP[full_num];
  double Bspline_px[full_num] , Bspline_py[full_num] , Bspline_u[full_num + order];
  double X[wp], Y[wp], Bspline_psid[wp];

  for (int u = 0; u<Npopu; u++) {
//    cout<<"vu="<<u<<endl;
    for (int j=0; j<num_wp_change; j++) {
      temp_x[j] = P[u][j];
      temp_y[j] = P[u][j+num_wp_change];
    }
    Construct_WP(x_WP, _x,temp_x ,index_change,num_wp_change,num_wp_total);
    Construct_WP(y_WP, _y,temp_y ,index_change,num_wp_change,num_wp_total);

    double L1=P[u][2*num_wp_change];
    double L2=P[u][2*num_wp_change+1];
//    cout<<"1"<<endl;
    //Bline_to_invert(Bspline_px,Bspline_py,Bspline_u,x_WP,y_WP,order,alpha,full_num);
//    cout<<"vu="<<endl;
//    cout<<"num_wp_total+num_wp_change="<<num_wp_total+num_wp_change<<endl;

    Bline_to_invert_heading(Bspline_px,Bspline_py,Bspline_u,x_WP,y_WP,order,alpha,num_wp_total+num_wp_change,start_yaw,end_yaw,L1,L2);
//    cout<<"vu="<<endl;
//    cout<<"1_1"<<endl;
    Vehinh(X, Y,Bspline_u,Bspline_px,Bspline_py,x_WP,y_WP,order,full_num, wp);
//    cout<<"vu="<<endl;
    double dx[wp], dy[wp],d2x_[wp],d2y_[wp],k[wp];

//    cout<<"2"<<endl;
    for (int i=0; i<wp-1; i++) {
      dy[i]=Y[i+1]-Y[i];
      dx[i]=X[i+1]-X[i];
    }
    for (int i=0; i<wp-2; i++) {
      d2y_[i]=dy[i+1]-dy[i];
      d2x_[i]=dx[i+1]-dx[i];
    }
    d2x_[wp-2] = 0;
    d2y_[wp-2] = 0;

    for (int i=0; i<wp-1; i++) {
      k[i]=(dx[i]*d2y_[i]-dy[i]*d2x_[i])/pow( (pow(dx[i],2)+pow(dy[i],2)),1.5);
    }

    double tmp_mm = k[0];
    double g1;
    //int ind_mm = 0;r
    for (int p =1; p<wp-1; p++) {
      if ( abs(k[p])>tmp_mm) {
        tmp_mm = abs(k[p]);
        //ind_mm= i;
      }
    }

    g1 = max_turn- tmp_mm;
    if (g1>0) g1 = 0;
//cout<<"3="<<endl;
//cout<<"num_Vc="<<num_Vc<<endl;
    g2 = 0;
    double tmp,val_k[2], dis[num_Vc];
    for (int m=0; m<num_Vc; m++) {
      dis[m]=99;
      for (double t0=0.01; t0<1; t0=t0+0.005) {
//cout<<"vu="<<endl;
        Fix_DEBOOR(val_k,Bspline_u,Bspline_px,Bspline_py,t0,order,full_num);
        tmp= sqrt(  pow(val_k[0]-Obs_x[m],2)+  pow(val_k[1]-Obs_y[m],2)  );
        if (tmp <dis[m]) dis[m] = tmp;
        //cout<<val_k[0]<<val_k[1]<<endl;
      //	cout<<t0<<endl;

      }
//        cout<<"dism<<"<<dis[m]<<endl;
      dis[m]=dis[m]-Obs_R[m];
      if (dis[m]>0 )  dis[m] =0;
      g2 = g2+dis[m];
    }
//    if(Npopu==1);
    // cout<<"g1:"<<g1<<endl;

  //	cout<<Obs_x[0];
  //	getchar();

    g=g1+g2;
    temp_cstr[u] =g;

    if (g<0 )
      temp_fit[u] = 1;
    else {
      S=0;
      for (int j= 2; j<wp-1; j++)
        S=S + sqrt(pow(X[j]-X[j-1],2) + pow(Y[j]-Y[j-1],2) );
      temp_S[u]=S;
      result[u]=S;
      temp_mat[ind_tmp_mat]= S;
      ind_tmp_mat ++;
    }
//    cout<<"g3:"<<g2<<endl;
  }

  for (int p =0; p<Npopu-1; p++) {
    if ( abs(temp_S[p])>max_S_val) {
      max_S_val = abs(temp_S[p]);
      //ind_mm= i;
    }
  }

  if (max_S_val == 0) //toan infeasible
    max_S_val = 2;
  else {
    for (int p =0; p<ind_tmp_mat; p++) {
      if ( abs(temp_mat[p])<min_S) {
        min_S = abs(temp_mat[p]);
        //ind_mm= i;
      }
    }
//    cout<<"minS:"<<min_S;
  }

  for (int u = 0; u<Npopu; u++) {
    if (temp_fit[u] == 1 ) { //Neu la ca the infeasible
      //	cout<<"toan inf 1 ";
      if (max_S_val !=  2 ) { // co them feasible
        result[u]= max_S_val -  temp_cstr[u]/(0.5*max_turn);
        //	cout<<"toan inf 2 ";
        if (result[u] > 2+ max_S_val)
          result[u] = 2 + max_S_val;

      } else { //toan infeasible
        result[u]= max_S_val -  2*temp_cstr[u];
        //	cout<<"toan inf 3";
        if (result[u] > 1000)
          result[u] = 1000;

      }

    }

  }
//  cout<<"g4:"<<g2<<endl;

}

double PathPlanningBsplines::getMax_Curvature()
{
  int wp = 1000;
  double dx[wp], dy[wp],d2x_[wp],d2y_[wp],k[wp];
    for (int i=0; i<wp-1; i++) {
      dy[i]=val_y[i+1]-val_y[i];
      dx[i]=val_x[i+1]-val_x[i];
//      cout<<"kdx[i]="<<dx[i]<<endl;
    }
    for (int i=0; i<wp-2; i++) {
      d2y_[i]=dy[i+1]-dy[i];
      d2x_[i]=dx[i+1]-dx[i];
//      cout<<"d2x_[i]="<<d2x_[i]<<endl;
    }
    d2x_[wp-2] = 0;
    d2y_[wp-2] = 0;
    for (int i=0; i<wp-1; i++) {
      k[i]=(dx[i]*d2y_[i]-dy[i]*d2x_[i])/pow( (pow(dx[i],2)+pow(dy[i],2)),1.5);
    }
    double tmp_mm = k[0];
    //int ind_mm = 0;
    for (int p =1; p<wp-1; p++) {
    //	cout<<abs(k[p])<<endl;
      if ( abs(k[p])>tmp_mm) {
        tmp_mm = abs(k[p]);
        //ind_mm= i;

      }
//      cout<<"tmp_mm"<<tmp_mm<<endl;
    }
    return tmp_mm;
  }
