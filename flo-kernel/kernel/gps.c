#include <linux/fs.h>
#include <linux/gps.h>
#include <linux/slab.h>
#include <linux/namei.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/syscalls.h>

static DEFINE_RWLOCK(cur_gloc_lock);

static struct gps_location cur_gloc = {
	.latitude = 0,
	.longitude = 0,
	.accuracy = 0
};

static long upd_tim_stm;

int kget_gps_location(struct gps_location *rloc, long *ts) {	
	read_lock(&cur_gloc_lock);
	memcpy(rloc, &cur_gloc, sizeof(cur_gloc));
	*ts = upd_tim_stm;
	read_unlock(&cur_gloc_lock);

	return 0;
}

SYSCALL_DEFINE1(set_gps_location, struct gps_location __user *, loc) {
	struct gps_location tmp_loc;

	if (current_uid() != 0)
		return -EACCES;

	if (loc == NULL)
		return -EINVAL;

	if (copy_from_user(&tmp_loc, loc, sizeof(tmp_loc)))
		return -EFAULT;

	write_lock(&cur_gloc_lock);
	memcpy(&cur_gloc, &tmp_loc, sizeof(tmp_loc));
	upd_tim_stm = CURRENT_TIME_SEC.tv_sec;
	write_unlock(&cur_gloc_lock);

	return 0;
}

SYSCALL_DEFINE2(get_gps_location, const char __user *, path_name, struct gps_location __user *,loc) {
	struct gps_location tmp_loc;
	struct path fp;
	struct inode *inode;
	int cage;
	int status;

	if (loc == NULL)
		return -EINVAL;

	if (user_path_at(AT_FDCWD, path_name, AT_SYMLINK_FOLLOW | !LOOKUP_FOLLOW, &fp))
		return -EFAULT;

	inode = fp.dentry->d_inode;
	if(inode_permission(inode, MAY_READ) < 0)
		return -EACCES;
	
	cage = vfs_get_gps_location(inode, &tmp_loc);
	status = copy_to_user(loc, &tmp_loc, sizeof(tmp_loc));
	if (status < 0)
		return -EFAULT;

	return cage;
}