#pragma once


typedef struct {
    double lat = 0;
    double lon = 0;

} GPS;

typedef struct {
    double N[3] = {0};
    double E[3] = {0};
} DMS;

GPS dms2gps(const DMS &dms);

DMS gps2dms(const GPS &gps);
