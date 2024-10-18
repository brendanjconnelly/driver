#include "fops.h"
#include "vd.h"
#include "char_driver.h"

#include <asm/uaccess.h>
#include <linux/string.h>

int read_ptr = 0;
int rc = 0;

ssize_t drv_read(struct file *file, char __user *user, size_t bytes, loff_t *loff) {
    if(driver_data[read_ptr] == '\0') {
        read_ptr = 0;
        return 0;
    }
    rc = copy_to_user(user, driver_data + read_ptr, 1);
    if(rc != 0) return rc;
    read_ptr++;
    return 1;
}

ssize_t drv_write(struct file *file, const char __user *user, size_t bytes, loff_t *loff) {
    return 0;
}

char *devnode_rwrwrw(struct device *dev, umode_t *mode) {
    *mode = 0666;
    
    return NULL;
}