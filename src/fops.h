#ifndef FOPS_H
#define FOPS_H

#include <linux/types.h>
#include <linux/fs.h>
#include <linux/device.h>

ssize_t drv_read(struct file *file, char __user *user, size_t bytes, loff_t *loff);
ssize_t drv_write(struct file *file, const char __user *user, size_t bytes, loff_t *loff);
char *devnode_rwrwrw(struct device *dev, umode_t *mode);

#endif