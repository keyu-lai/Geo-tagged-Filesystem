#include "gpsd.h"

int main(int argc, char *argv[])
{
	while (1) {
		pid_t pid;

		pid = fork();

		if (pid < 0) {
			perror("Error! Fail to fork.\n");
			exit(EXIT_FAILURE);
		}

		if (pid > 0) {
			exit(EXIT_SUCCESS);
		}

		if (setsid() < 0) {
			perror("Error ! Fail to setsid\n");
			exit(EXIT_FAILURE);
		}

		umask(0);
		poll_gps();

		close(0);
		close(1);
		close(2);
	}

	return 0;
}

static int poll_gps() {
	double latitude;
	double longtitude;
	float accuracy;

	struct gps_location loc;
	FILE *fd = fopen(GPS_LOCATION_FILE, "r");

	if (!fp){
		perror("Error! Can not open gps_location file.\n");
		return -1;
	}

	fscanf(fd, "%f %f %f",lattitude,longtitude, accuracy);
	loc.latitude = latitude;
	loc.longtitude = longtitude;
	loc.accuracy = accuracy;

	set_gps_location(loc);

	fclose();

	return 0;
}

