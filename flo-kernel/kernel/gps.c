#include <linux/fs.h>
#include <linux/gps.h>
#include <linux/slab.h>
#include <linux/namei.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/syscalls.h>
#include <linux/errno.h>
#include <linux/dcache.h>

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
	//int luf;
	int status;
	int cage = 0;
	long accuracy;
	char *from;

	printk(KERN_DEBUG "Enter get_gps_location syscall\n");
	memcpy(&accuracy, &cur_gloc.accuracy,sizeof(accuracy));
	printk(KERN_DEBUG "Accuracy:%ld\n",accuracy);

	if (loc == NULL) {
		printk(KERN_DEBUG "NULL loc\n");
		return -EINVAL;
	}

	if (accuracy == -1) {
		printk(KERN_DEBUG "wrong accuracy\n");
		return -ENODEV;
	}
	
	from = getname(path_name);
	if (IS_ERR(from)) {
		printk(KERN_DEBUG "Wrong name!");
	}

	printk("%s::%d\n",path_name,strlen(path_name));
	//luf = 0;
	//luf |= (LOOKUP_FOLLOW);
	//if (user_path_at(AT_FDCWD, path_name, luf, &fp) < 0) {
	if (kern_path(path_name, LOOKUP_FOLLOW | LOOKUP_AUTOMOUNT, &fp) != 0) {
		printk(KERN_DEBUG "wrong path\n");
		return -EFAULT;
	}

	inode = fp.dentry->d_inode;
	if(inode_permission(inode, MAY_READ) < 0) {
		printk(KERN_DEBUG "No Permission\n");
		return -EACCES;
	}

	cage = vfs_get_gps_location(inode, &tmp_loc);
	status = copy_to_user(loc, &tmp_loc, sizeof(tmp_loc));
	if (status < 0) {
		printk(KERN_DEBUG "Negative Status\n");
		return -EFAULT;
	}

	return cage;
}