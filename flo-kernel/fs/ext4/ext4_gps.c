#include <linux/gps.h>
#include <linux/time.h>
#include "ext4.h"

int ext4_set_gps_location(struct inode *inode) {
	struct gps_location loc;
	struct ext4_inode_info *iinfo = EXT4_I(inode);
	long ts;

	kget_gps_location(&loc, &ts);
	ts = CURRENT_TIME_SEC.tv_sec - ts;
	
	write_lock(&iinfo->i_gps_lock);
	memcpy(&iinfo->i_latitude, &loc.latitude, sizeof(unsigned long));
	memcpy(&iinfo->i_longitude, &loc.longitude, sizeof(unsigned long));
	memcpy(&iinfo->i_accuracy, &loc.accuracy, sizeof(unsigned int));
	memcpy(&iinfo->i_coord_age, &ts, sizeof(unsigned int));
	write_unlock(&iinfo->i_gps_lock);

	return 0;
}

int ext4_get_gps_location(struct inode *inode, struct gps_location *location)
{
	struct gps_location tmp_loc;
	struct ext4_inode_info *iinfo = EXT4_I(inode);
	int coord_age;

	read_lock(&iinfo->i_gps_lock);
	memcpy(&tmp_loc.latitude, &iinfo->i_latitude, sizeof(unsigned long));
	memcpy(&tmp_loc.longitude, &iinfo->i_longitude, sizeof(unsigned long));
	memcpy(&tmp_loc.accuracy, &iinfo->i_accuracy, sizeof(unsigned int));
	memcpy(location, &tmp_loc, sizeof(tmp_loc));
	coord_age = *(int *) &iinfo->i_coord_age;
	read_unlock(&iinfo->i_gps_lock);

	return coord_age;
}