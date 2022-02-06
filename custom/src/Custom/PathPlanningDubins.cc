#include "PathPlanningDubins.h"

PathPlanningDubins::PathPlanningDubins(QObject*)
{
}

PathPlanningDubins::~PathPlanningDubins(){

}

void PathPlanningDubins::clear(){
    path_latlng.clear();
}

void PathPlanningDubins::los_dubins_main(QList<QGeoCoordinate> waypointList)
{
    wp_latlng_in.resize(waypointList.length());
    wp_xy_in.resize(waypointList.length());

    for(int i = 0; i < waypointList.length(); i++){
        wp_latlng_in[i].x = waypointList.value(i).latitude();
        wp_latlng_in[i].y = waypointList.value(i).longitude();
    }
  numIn = waypointList.length();
  setBase(10.763915776641571,106.65688877139627);
  Dubins_Mapgui();
}

void PathPlanningDubins::Dubins_Mapgui()
{
  // ar_in_lat, lng: Waypoint dau vao theo lat lng
  // num_wp_in: so luong waypoint
  // b_lat, b_lng: base cua GPS
  // ar_out_lat, lng: Waypoint dau ra theo lat lng
  // ar_in_latlng: Waypoint dau vao theo lat lng
  // base_ll: base cua GPS
  // ar_out_ll: Waypoint dau ra theo lat lng
  for (int i = 0; i < numIn; i++)
    ConverttoXY(wp_latlng_in[i].x, wp_latlng_in[i].y, base_latlng.x, base_latlng.y,
                wp_xy_in[i].x, wp_xy_in[i].y);

  for (int i = 0; i < num_Vc; i++)
      ConverttoXY(Obs_lat[i], Obs_lng[i], base_latlng.x, base_latlng.y,
                  Obs_x[i], Obs_y[i]);

  F_Dubins_Path_Planning();
  wp_latlng_out.resize(numOut);

  for (int i = 0; i < numOut; i++)
  {
    ConverttoLatLong(wp_xy_out[i].x, wp_xy_out[i].y, base_latlng.x, base_latlng.y,
                     wp_latlng_out[i].x, wp_latlng_out[i].y);
  }

  path_latlng.resize(path_xy.size());
  for (int i = 0; i < path_xy.size(); i++)
  {
    ConverttoLatLong(path_xy[i].x, path_xy[i].y, base_latlng.x, base_latlng.y,
                     path_latlng[i].x, path_latlng[i].y);
  }
}

void PathPlanningDubins::F_Cal_Parametrization_Arc(double& t, double& anpha, Point P1,
                                           Point P2, Point P3, int rho)
{
  // P1,P2,P3   = The given points
  // checked
  // *****Output*****
  // angle  = The angle of (P1,P2,P3)

  Point v_in, v_out;
  v_in.x = (P1.x - P2.x) / hypot(P1.x - P2.x, P1.y - P2.y);
  v_in.y = (P1.y - P2.y) / hypot(P1.x - P2.x, P1.y - P2.y);

  v_out.x = (P3.x - P2.x) / hypot(P3.x - P2.x, P3.y - P2.y);
  v_out.y = (P3.y - P2.y) / hypot(P3.x - P2.x, P3.y - P2.y);
  int direc = -sign(v_in.y*v_out.x-v_in.x*v_out.y);
  t = acos(scalar_product(v_in, v_out));
  if ((rho*direc < 0)&&(t>0.2))
      t = 2*M_PI-acos(scalar_product(v_in, v_out));
  anpha = atan2((P1.y - P2.y), (P1.x - P2.x));
}

void PathPlanningDubins::F_Circle_Circle_Tangent(Point& P1, Point& P2, Point O1,
                                         Point O2, double R1, double R2, int flag, int rho)
{
  // O1   = Origin of the Circle 1
  // O2   = Origin of the Circle 2
  // R    = Radius of the circle
  // flag = Determine cross tangent (flag = -1) or along tangent (flag = 1)
  // rho  = Sign of tangent point

  // P1  = Tangent position on circle 1
  // P2  = Tangent position on circle 2
    double d = hypot(O2.x-O1.x, O2.y-O1.y);
    Point unitTangent, unitNormal;
    unitTangent.x = (O2.x-O1.x) / d;
    unitTangent.y = (O2.y-O1.y) / d;
    unitNormal.x = (O1.y-O2.y) / d;
    unitNormal.y = (O2.x-O1.x) / d;
    if (flag == -1){
        // Internal Common Tangents
        double theta = acos((R1+R2)/d);
        if (rho==-1)
        {
            P1.x=O1.x+unitTangent.x*R1*cos(theta)+unitNormal.x*R1*sin(theta);
            P1.y=O1.y+unitTangent.y*R1*cos(theta)+unitNormal.y*R1*sin(theta);
            P2.x=O2.x-unitTangent.x*R2*cos(theta)-unitNormal.x*R2*sin(theta);
            P2.y=O2.y-unitTangent.y*R2*cos(theta)-unitNormal.y*R2*sin(theta);
        }
        else
        {
            P1.x=O1.x+unitTangent.x*R1*cos(theta)-unitNormal.x*R1*sin(theta);
            P1.y=O1.y+unitTangent.y*R1*cos(theta)-unitNormal.y*R1*sin(theta);
            P2.x=O2.x-unitTangent.x*R2*cos(theta)+unitNormal.x*R2*sin(theta);
            P2.y=O2.y-unitTangent.y*R2*cos(theta)+unitNormal.y*R2*sin(theta);
        }
    }
    else
        {
            double theta = acos((R1-R2)/d);
            if (rho == -1)
            {
                P1.x = O1.x+unitTangent.x*R1*cos(theta)+unitNormal.x*R1*sin(theta);
                P1.y = O1.y+unitTangent.y*R1*cos(theta)+unitNormal.y*R1*sin(theta);
                P2.x = O2.x+unitTangent.x*R2*cos(theta)+unitNormal.x*R2*sin(theta);
                P2.y = O2.y+unitTangent.y*R2*cos(theta)+unitNormal.y*R2*sin(theta);
            }
            else
            {
                P1.x=O1.x+unitTangent.x*R1*cos(theta)-unitNormal.x*R1*sin(theta);
                P1.y=O1.y+unitTangent.y*R1*cos(theta)-unitNormal.y*R1*sin(theta);
                P2.x=O2.x+unitTangent.x*R2*cos(theta)-unitNormal.x*R2*sin(theta);
                P2.y=O2.y+unitTangent.y*R2*cos(theta)-unitNormal.y*R2*sin(theta);
            }
        }
}

void PathPlanningDubins::F_Circle_Through_Waypoint(Point& p, int& rho, Point WP1,
                                           Point WP2, Point WP3, double R)
{
  // *****Input*****
  // Waypoint include 3 node: WP1 WP2 WP3
  // R   = Radius of the circle
  // p   = The column vector of origin circle position
  // rho = The rotation direction of WP1 -> WP2 -> WP3

  Point v_in, v_out;
  Point h;
  v_in.x = (WP2.x - WP1.x) / hypot(WP2.y - WP1.y, WP2.x - WP1.x);
  v_in.y = (WP2.y - WP1.y) / hypot(WP2.y - WP1.y, WP2.x - WP1.x);

  v_out.x = (WP3.x - WP2.x) / hypot(WP3.y - WP2.y, WP3.x - WP2.x);
  v_out.y = (WP3.y - WP2.y) / hypot(WP3.y - WP2.y, WP3.x - WP2.x);

  rho = -sign(v_in.y * v_out.x - v_in.x * v_out.y);
  if (rho == 0)
    rho += 1;

  // The half of inner corner of vertex WP2
  double anpha = rho * acos(-v_in.x * v_out.x - v_in.y * v_out.y) / 2;

  h.x = R * (WP3.x - WP2.x) / hypot(WP3.y - WP2.y, WP3.x - WP2.x);
  h.y = R * (WP3.y - WP2.y) / hypot(WP3.y - WP2.y, WP3.x - WP2.x);

  p.x = cos(anpha) * h.x - sin(anpha) * h.y + WP2.x;
  p.y = sin(anpha) * h.x + cos(anpha) * h.y + WP2.y;
}

void PathPlanningDubins::F_Point_Circle_Tangent(Point& pt, Point wp, Point O, double R,
                                        int rho)
{
  // WP  = The given point
  // O   = The origin of circle
  // R   = Radius of the circle
  // rho = Sign of tangent point

  // p_t = The column vector of tangent position
  // The distance from given point to origin
  double p = hypot(O.y - wp.y, O.x - wp.x);
  // Rotation angle
  double anpha = -rho * asin(R / p);
  // Lenght of the tangent
  double l_t = sqrt(pow(p, 2) - pow(R, 2));
  //%% Rotation
  Point h;
  h.x = l_t * (O.x - wp.x) / hypot(O.y - wp.y, O.x - wp.x);
  h.y = l_t * (O.y - wp.y) / hypot(O.y - wp.y, O.x - wp.x);

  pt.x = cos(anpha) * h.x - sin(anpha) * h.y + wp.x;
  pt.y = sin(anpha) * h.x + cos(anpha) * h.y + wp.y;
}

void PathPlanningDubins::F_Dubins_Path_Planning()
{
  path_length = 0;
  // Input*****
  // wp_xy_in    = Input waypoints given by user
  // R_min = The radius relative to max curvature

  // path = Desired position of Dubins path
  // WP_out The waypoint out which is the borders of line and arc in Dubins path
  // po = The origin of arcs in Dubins path
  // rho The direction of arcs in Dubins path (1 = clockwise, -1 = anticlockwise)
  qDebug()<<num_Vc;
  Point Obs_Point[num_Vc];

  for(int i=0;i<num_Vc;i++){
      Obs_Point[i].x = Obs_x[i];
      Obs_Point[i].y = Obs_y[i];
  }
  for (int i = 0; i < numIn - 2; i++)
      F_Circle_Through_Waypoint(p_o[i], rho[i], wp_xy_in[i], wp_xy_in[i + 1], wp_xy_in[i + 2], R_min);

  // Calculate the tangent point
  Point p_1[numIn - 1];
  Point p_2[numIn - 1];
  //p1: start of line - end of arc
  //p2: start of arc - end of line

  p_1[0] = wp_xy_in[0];
  F_Point_Circle_Tangent(p_2[0], wp_xy_in[0], p_o[0], R_min, rho[0]);

  // Calculate the the last tangent point
  F_Point_Circle_Tangent(p_1[numIn-2], wp_xy_in[numIn - 1], p_o[numIn - 3], R_min,-rho[numIn - 3]);
  p_2[numIn-2] = wp_xy_in[numIn-1];

  for (int i = 0; i < numIn - 3; i++)
  {
    int flag = rho[i] * rho[i + 1];
    F_Circle_Circle_Tangent(p_1[i+1], p_2[i+1], p_o[i], p_o[i + 1], R_min, R_min, flag,rho[i]);
  }

  vector <double> R_out;
  vector < vector <double> >avoid;
  avoid.resize(numIn-1);
  for(size_t i=0;i<numIn-1;i++){
      avoid[i].resize(num_Vc);
      for(size_t j=0;j<num_Vc;j++)
      {
          avoid[i][j] = 1;
      }
  }

  for(size_t i=0;i<numIn-1;i++)
    {
        double alpha = atan2(p_2[i].y-p_1[i].y,p_2[i].x-p_1[i].x);
        for(size_t j=0;j<num_Vc;j++)
        {
            double dx = (Obs_x[j] - p_1[i].x)*cos(alpha)+(Obs_y[j] - p_1[i].y)*sin(alpha);
            double dy = -(Obs_x[j]-p_1[i].x)*sin(alpha)+(Obs_y[j]-p_1[i].y)*cos(alpha);
            if ((dx>0)&&(dx<hypot(p_2[i].y-p_1[i].y,p_2[i].x-p_1[i].x))&&(fabs(dy)<Obs_R[j]+1.0))
                avoid[i][j] = 1;
            else avoid[i][j] = 0;
        }
    }


    vector <Point> p_oo, p_11, p_22;
    vector <int> rhoo;
    vector <double> t_arc, anpha_arc;
    for(size_t i=0;i<numIn-1;i++){
        for(size_t j=0;j<num_Vc;j++){
            if (avoid[i].at(j) == 1){
                p_oo.push_back(Obs_Point[j]);
                if (i<numIn-2)
                    rhoo.push_back(rho[i]);
                else
                    rhoo.push_back(1);
                R_out.push_back(max(Obs_R[j]+1.0,R_min));
            }
        }
        if (i<numIn-2)
        {
            p_oo.push_back(p_o[i]);
            rhoo.push_back(rho[i]);
            R_out.push_back(R_min);
        }
  }

    p_11.push_back(wp_xy_in[0]);
    Point tmp_p11, tmp_p22;
    F_Point_Circle_Tangent(tmp_p22,wp_xy_in[0],p_oo.front(),R_out.front(),rhoo.front());
    p_22.push_back(tmp_p22);
    for(size_t i=0;i<p_oo.size()-1;i++)
    {
        int flag = rhoo[i]*rhoo.at(i+1);
        F_Circle_Circle_Tangent(tmp_p11,tmp_p22,p_oo[i],p_oo.at(i+1),R_out[i],R_out.at(i+1),flag,rhoo[i]);
        p_11.push_back(tmp_p11);
        p_22.push_back(tmp_p22);
    }
    F_Point_Circle_Tangent(tmp_p11,wp_xy_in[numIn-1],p_oo.back(),R_out.back(),-rhoo.back());
    p_11.push_back(tmp_p11);
    p_22.push_back(wp_xy_in[numIn-1]);

    for (int i = 0; i < p_11.size(); i++)
    {
        wp_xy_out.push_back(p_11[i]);
        wp_xy_out.push_back(p_22[i]);
    }
    numOut = wp_xy_out.size();
    // Calculate desired x,y-position for plot
    // Calculate parametrization for arcs on circles include domain [0,t_arc(i)]
    // and initial tangent angles (anpha_arc(i))

    t_arc.resize(p_oo.size());
    anpha_arc.resize(p_oo.size());

    for (int i = 0; i < p_oo.size(); i++)
      F_Cal_Parametrization_Arc(t_arc[i], anpha_arc[i], p_22[i], p_oo[i], p_11[i+1],rhoo[i]);

  //Tinh chieu dai quang duong
  for (int i = 0; i<numOut-1; i++)
      path_length += hypot(wp_xy_out[i+1].y-wp_xy_out[i].y,wp_xy_out[i+1].x-wp_xy_out[i].x);

  path_xy.push_back(wp_xy_out[0]);
  for (int i = 0; i < p_oo.size(); i++)
  {
      path_length += R_out[i]*t_arc[i];
      double theta;
      for (size_t l = 0; l <= 99; l++)
      {
        theta = (l + 1) * t_arc[i] / 100;
        Point path_tmp;
        path_tmp.x = p_oo[i].x + R_out[i] * cos(rhoo[i] * theta + anpha_arc[i]);
        path_tmp.y = p_oo[i].y + R_out[i] * sin(rhoo[i] * theta + anpha_arc[i]);
        path_xy.push_back(path_tmp);
      }
  }
  path_xy.push_back(wp_xy_out.back());
  if (isnan(path_length))
          path_length = 10000.0;
}

double PathPlanningDubins::scalar_product(Point V1, Point V2)
{
  return V1.x * V2.x + V1.y * V2.y;
}
