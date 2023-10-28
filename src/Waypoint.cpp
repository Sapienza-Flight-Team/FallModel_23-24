#include "Waypoint.h"
#include <cmath>
GPS dms2gps(const DMS &dms) {
  GPS ret;
  for (int i = 0; i < 3; i++) {
    ret.lat += dms.N[i] / pow(60, i);
    ret.lon += dms.E[i] / pow(60, i);
  }
  return ret;
}

GPS translate_gps(const GPS &gps, double d, double head) {
  double R_E = 6378100; // Earth radius (m)
  if (d < 0) {
    d = -d;
    head += 180;
  }
  double head_rad = head * M_PI / 180;   // Convert heading to rad
  double lat_rad = gps.lat * M_PI / 180; // Convert latitude to rad
  double lon_rad = gps.lon * M_PI / 180; // Convert longitude to rad

  double lat_new = asin(sin(lat_rad) * cos(d / R_E) +
                        cos(lat_rad) * sin(d / R_E) * cos(head_rad));
  double lon_new = lon_rad + atan2(sin(head_rad) * sin(d / R_E) * cos(lat_rad),
                                   cos(d / R_E) - sin(lat_rad) * sin(lat_new));

  GPS gps_new;
  gps_new.lat = lat_new * 180 / M_PI; // Convert latitude back to degree
  gps_new.lon = lon_new * 180 / M_PI; // Convert longitude back to degree

  return gps_new;
}

DMS gps2dms(const GPS &gps) {
  DMS ret;

  ret.N[0] = floor(gps.lat);
  ret.E[0] = floor(gps.lon);

  GPS temp{(gps.lat - floor(gps.lat)) * 60, (gps.lon - floor(gps.lon)) * 60};
  ret.N[1] = floor(temp.lat);
  ret.E[1] = floor(temp.lon);

  ret.N[2] = (temp.lat - floor(temp.lat)) * 60;
  ret.E[2] = (temp.lon - floor(temp.lon)) * 60;

  return ret;
}