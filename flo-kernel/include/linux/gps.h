#ifndef _LINUX_GPS_H
#define _LINUX_GPS_H

struct gps_location {
	double latitude;
	double longitude;
	float  accuracy;  /* in meters */
};

int kget_gps_location(struct gps_location *,long *);
#endif