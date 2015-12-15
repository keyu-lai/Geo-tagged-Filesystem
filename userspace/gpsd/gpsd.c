#include "gpsd.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#define LIN_LEN 100

static int poll_gps(void)
{
	double latitude;
	double longitude;
	float accuracy;
	char str[100];

	struct gps_location loc;
	FILE *fd = fopen(GPS_LOCATION_FILE, "r");

	if (!fd) {
		perror("Error! Can not open gps_location file.\n");
		accuracy = -1;
	}

	if (fgets(str, LIN_LEN, fd) != NULL)
		sscanf(str, "%lf", &latitude);
	else
		accuracy = -1;

	if (fgets(str, LIN_LEN, fd) != NULL)
		sscanf(str, "%lf", &longitude);
	else
		accuracy = -1;

	if (fgets(str, LIN_LEN, fd) != NULL)
		sscanf(str, "%f", &accuracy);
	else
		accuracy = -1;

	fclose(fd);

	loc.latitude = latitude;
	loc.longitude = longitude;
	loc.accuracy = accuracy;

	set_gps_location(&loc);

	return 0;
}

int main(int argc, char *argv[])
{
	pid_t pid;

	pid = fork();

	if (pid < 0) {
		perror("Error! Fail to fork.\n");
		exit(EXIT_FAILURE);
	}

	if (pid > 0)
		exit(EXIT_SUCCESS);

	if (setsid() < 0) {
		perror("Error ! Fail to setsid\n");
		exit(EXIT_FAILURE);
	}
	chdir("/data/misc");
	umask(0);
	printf("%d", pid);
	close(0);
	close(1);
	close(2);

	while (1) {
		poll_gps();
		usleep(1000);
	}

	return 0;
}

