//Bien vao waypointList;
//Bien ra WP_out;
#define Earth_radius 6371000

#include "TSP_Basic.h"
#include "Custom/PathPlanningDubins.h"

void TSP_Basic::inputWaypoints(QList<QGeoCoordinate> waypointList){
    for(int i = 0; i < waypointList.length(); i++){
        waypoints[i].x = waypointList.value(i).latitude();
        waypoints[i].y = waypointList.value(i).longitude();
    }
}
