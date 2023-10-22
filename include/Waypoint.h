#include <cmath>

typedef struct {
    double lat = 0;
    double lon = 0;


} GPS;

typedef struct {
    double N[3] = {0};
    double E[3] = {0};
} DMS;

GPS dms2gps(const DMS &dms) {
    GPS ret;
    for (int i = 0; i < 3; i++) {
        ret.lat += dms.N[i] / pow(60, i + 1);
        ret.lon += dms.E[i] / pow(60, i + 1);
    }
    return ret;
}

DMS gps2dms(const GPS &gps) {
    DMS ret;
    
    ret.N[0] = floor(gps.lat);
    ret.E[0] = floor(gps.lon);
    
    GPS temp{(gps.lat - floor(gps.lat)) * 60, (gps.lon - floor(gps.lon)) * 60};
    ret.N[1] = floor(temp.lat);
    ret.E[1] = floor(temp.lon);

    ret.N[2] = (temp.lat - floor(temp.lat))*60;
    ret.E[2] = (temp.lon - floor(temp.lon))*60;

    return ret;
}