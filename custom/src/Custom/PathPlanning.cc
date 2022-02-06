#include "PathPlanning.h"

PathPlanning::PathPlanning()
{
}

PathPlanning::~PathPlanning(){
}

void PathPlanning::ConverttoXY(double lat, double lon, double ref_lat,
                               double ref_lon, double& x, double& y){
    if ((lat == ref_lat) && (lon == ref_lon))
      x = y = 0;

    double lat_rad = lat * M_PI / 180.0;
    double lon_rad = lon * M_PI / 180.0;

    double ref_lon_rad = ref_lon * M_PI / 180.0;
    double ref_lat_rad = ref_lat * M_PI / 180.0;

    double sin_lat = sin(lat_rad);
    double cos_lat = cos(lat_rad);
    double cos_d_lon = cos(lon_rad - ref_lon_rad);

    double ref_sin_lat = sin(ref_lat_rad);
    double ref_cos_lat = cos(ref_lat_rad);

    double c = acos(ref_sin_lat * sin_lat + ref_cos_lat * cos_lat * cos_d_lon);
    double k =
        (fabs(c) < std::numeric_limits<double>::epsilon()) ? 1.0 : (c / sin(c));

    x = k * (ref_cos_lat * sin_lat - ref_sin_lat * cos_lat * cos_d_lon) * 6371000;
    y = k * cos_lat * sin(lon_rad - ref_lon_rad) * 6371000;
}

void PathPlanning::ConverttoLatLong(double x, double y, double ref_lat,
                                    double ref_lon, double& lat, double& lon)
  {
    double x_rad = x / Earth_radius;
    double y_rad = y / Earth_radius;
    double c = sqrt(x_rad * x_rad + y_rad * y_rad);
    double sin_c = sin(c);
    double cos_c = cos(c);

    double ref_lon_rad = ref_lon * M_PI / 180.0;
    double ref_lat_rad = ref_lat * M_PI / 180.0;

    double ref_sin_lat = sin(ref_lat_rad);
    double ref_cos_lat = cos(ref_lat_rad);

    double lat_rad;
    double lon_rad;

    if (fabs(c) > std::numeric_limits<double>::epsilon())
    {
      lat_rad = asin(cos_c * ref_sin_lat + (x_rad * sin_c * ref_cos_lat) / c);
      lon_rad =
          (ref_lon_rad + atan2(y_rad * sin_c, c * ref_cos_lat * cos_c -
                                                  x_rad * ref_sin_lat * sin_c));
    }
    else
    {
      lat_rad = ref_lat_rad;
      lon_rad = ref_lon_rad;
    }

    lat = lat_rad * 180.0 / M_PI;
    lon = lon_rad * 180.0 / M_PI;
  }

void PathPlanning::setBase(double baseLat, double baseLng)
{
  this->base_latlng.x = baseLat;
  this->base_latlng.y = baseLng;
}
