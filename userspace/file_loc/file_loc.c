#include "file_loc.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

#define GPS_LOCATION_FILE "/data/media/0/gps_location.txt"
#define STR_LEN 200

int main(int argc, char **argv)
{
	char *opt, *path;
	char copt[STR_LEN];
	int dage, i, slen;
	size_t br = STR_LEN;
	struct gps_location rloc;

	opt = (char *)malloc(STR_LEN * sizeof(char));

	while (strncmp(copt, "exit", 4) != 0) {
		printf("Enter path name: ");
		slen = getline(&opt, &br, stdin);
		
		if (slen > 0) {
			path = (char *)malloc(slen * sizeof(char));
			strncpy(path, opt, slen - 1);
			path[slen - 1] = '\0';
		}

		i = 0;
		while (i < STR_LEN) {
			copt[i] = tolower(opt[i]);
			i++;
		}

		if (strncmp(copt, "exit", 4) != 0) {
			printf("Path: %s\n",path);
			rloc.latitude = 0;
			rloc.longitude = 0;
			rloc.accuracy = 0;

			dage = get_gps_location(path,&rloc);
			free(path);

			if (dage < 0 && strncmp(copt, "exit", 4) != 0) {
				printf("Error! Error code is %d.\n",dage);
				continue;
			}

			printf("File name: %s\n",opt);
			printf("Created at lat : %f\n",rloc.latitude);
			printf("Created at lod : %f\n",rloc.longitude);
			printf("Data Accuracy: %f\n",rloc.accuracy);
			printf("Data age: %d\n",dage);
		}
	}
	free(opt);

	return 0;
}
