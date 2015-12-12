#include "file_loc.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define GPS_LOCATION_FILE "/data/media/0/gps_location.txt"
#define STR_LEN 200

int main(int argc, char **argv)
{
	char *opt;
	char copt[STR_LEN];
	int dage, i;
	size_t nbytes = STR_LEN;
	struct gps_location rloc;

	opt = (char *)malloc(STR_LEN * sizeof(char));

	while (strcmp(copt, "exit") != 0) {
		printf("Enter path name: ");
		getline(&opt, &nbytes, stdin);
		
		dage = get_gps_location(opt,&rloc);
		
		printf("File name: %s\n",opt);
		printf("Created at lat : %f\n",rloc.latitude);
		printf("Created at lod : %f\n",rloc.longitude);
		printf("Data Accuracy: %f\n",rloc.accuracy);
		printf("Data age: %d\n",dage);

		i = 0;
		while (i < STR_LEN) {
			copt[i] = tolower(opt[i]);
			i++;
		}
	}
	free(opt);

	return 0;
}
