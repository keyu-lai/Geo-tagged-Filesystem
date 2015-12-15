#include <linux/gps.h>
#include <linux/time.h>
#include "ext4.h"

long ext4_set_gps_location(struct inode *inode)
{
	struct gps_location loc;
	struct ext4_inode_info *iinfo = EXT4_I(inode);
	long ts;

	if (!test_opt(inode->i_sb, GPS_AWARE_INODE))
		return -ENODEV;
	kget_gps_location(&loc, &ts);
	ts = CURRENT_TIME_SEC.tv_sec - ts;

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
	__u64 longitude, latitude;
	__u32 accuracy;

	if (!test_opt(inode->i_sb, GPS_AWARE_INODE))
		return -ENODEV;


	read_lock(&iinfo->i_gps_lock);
	memcpy(&tmp_loc.latitude, &iinfo->i_latitude, sizeof(long long));
	memcpy(&tmp_loc.longitude, &iinfo->i_longitude, sizeof(long long));
	memcpy(&tmp_loc.accuracy, &iinfo->i_accuracy, sizeof(long));
	memcpy(location, &tmp_loc, sizeof(tmp_loc));
	coord_age = *(long *) &iinfo->i_coord_age;
	read_unlock(&iinfo->i_gps_lock);

	/* 
	 * this is a hack to handle the case of a file with no data.
	 * if someone really were visit the exact spot (0.0, 0.0)
	 * and their device were accurate down to 0.0, this would
 	 * be wrong. but that seems basically impossible
 	 */
	memcpy(&longitude, &tmp_loc.longitude, sizeof(__u64));
	memcpy(&latitude, &tmp_loc.latitude, sizeof(__u64));
	memcpy(&accuracy, &tmp_loc.accuracy, sizeof(__u32));
	if (latitude == 0 && longitude == 0 && accuracy == 0)
		return -ENODEV;

	return coord_age;
}