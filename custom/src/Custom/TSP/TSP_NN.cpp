//Bien vao waypointList;
//Bien ra WP_out;
#define Earth_radius 6371000

#include "TSP_NN.h"

void TSP_NN::TSP_NN_Run(QList<QGeoCoordinate> waypointList)
{
    setBase(10.763915776641571,106.65688877139627);
    N_WP = waypointList.length();
    for (int i=0;i<waypointList.length();i++)
    {
       ConverttoXY(waypointList.value(i).latitude(),waypointList.value(i).longitude(), base_latlng.x, base_latlng.y,
       waypoints[i].x, waypoints[i].y);
    }
    //diem bat dau va ket thuc la co dinh

    for(int i=0;i<N_WP;i++)
        for(int j=0;j<N_WP;j++)
        {
            if (i!=j)
                d[i][j] = hypot(waypoints[i].x-waypoints[j].x,waypoints[i].y-waypoints[j].y);
            else
                d[i][j] = 1000;
        }
    
    int present_point = 0;
    qDebug()<<"OK";
    vector <Point> tmpWP_out;
    tmpWP_out.push_back(waypoints[0]); //phan tu dau tien
    for(int i=1;i<N_WP-1;i++)
    {
        double min_d = 1000;
        int min_index = 0;
        for (int j=1;j<N_WP-1;j++)
            if(d[present_point][j] < min_d)
                {
                    min_d = d[present_point][j];
                    min_index = j;
                }
        tmpWP_out.push_back(waypoints[min_index]);
        present_point = min_index;
        for(int k=0;k<N_WP;k++)
            d[k][present_point] = 1000;
    }  
    tmpWP_out.push_back(waypoints[N_WP-1]);

    for (int i = 0; i < N_WP; i++){
        Point tmpWaypoint;
        ConverttoLatLong(tmpWP_out[i].x, tmpWP_out[i].y, base_latlng.x, base_latlng.y, tmpWaypoint.x, tmpWaypoint.y);
        WP_out.append(tmpWaypoint);
    }
}
