#include <linux/gps.h>
#include <linux/time.h>
#include "ext4.h"

long ext4_set_gps_location(struct inode *inode) {
	struct gps_location loc;
	struct ext4_inode_info *iinfo = EXT4_I(inode);
	long ts;

	kget_gps_location(&loc, &ts);
	ts = CURRENT_TIME_SEC.tv_sec - ts;
	if (!(EXT4_SB(inode->i_sb)->s_mount_flags & EXT4_MOUNT_GPS_AWARE_INODE))
		return 1;

	write_lock(&iinfo->i_gps_lock);
	memcpy(&iinfo->i_latitude, &loc.latitude, sizeof(long long));
	memcpy(&iinfo->i_longitude, &loc.longitude, sizeof(long long));
	memcpy(&iinfo->i_accuracy, &loc.accuracy, sizeof(long));
	memcpy(&iinfo->i_coord_age, &ts, sizeof(long));
	write_unlock(&iinfo->i_gps_lock);

	return 0;
}

long ext4_get_gps_location(struct inode *inode, struct gps_location *location)
{
	struct gps_location tmp_loc;
	struct ext4_inode_info *iinfo = EXT4_I(inode);
	long coord_age;

	//if (!(EXT4_SB(inode->i_sb)->s_mount_flags & EXT4_MOUNT_GPS_AWARE_INODE))
		//return 0;

	read_lock(&iinfo->i_gps_lock);
	memcpy(&tmp_loc.latitude, &iinfo->i_latitude, sizeof(long long));
	memcpy(&tmp_loc.longitude, &iinfo->i_longitude, sizeof(long long));
	memcpy(&tmp_loc.accuracy, &iinfo->i_accuracy, sizeof(long));
	memcpy(location, &tmp_loc, sizeof(tmp_loc));
	coord_age = *(long *) &iinfo->i_coord_age;
	read_unlock(&iinfo->i_gps_lock);

	return coord_age;
}