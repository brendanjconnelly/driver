#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <generated/utsrelease.h>

#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/errno.h>

#include "fops.h"
#include "char_driver.h"

dev_t dev = -1;
int dev_alloc_rc = 0;
char *driver_device_name = DRIVER_NAME__;

struct cdev *cdev;
struct file_operations fops;

struct class *class;
struct device *device_file;

#define DBG_PRINT() printk(KERN_INFO DRIVER_NAME "%s:%d\n", __func__, __LINE__);

/**
 * dev_t is the number used by the kernel to refer to this device
 * cdev is the struct representing the chrdev
 * the class is kind of like the bridge between the filesystem and the device
 */
static int __init __drv_init(void) {
    printk(KERN_INFO "-----------------------------------------------\n");
    printk(KERN_INFO DRIVER_NAME "INIT: BUILD NUMBER " TEST_DRV_VER "\n");
    printk(KERN_INFO DRIVER_NAME "KERNEL VERSION DETECTED: " UTS_RELEASE "\n");

    // File operations callback struct
    fops = (struct file_operations) {
        .owner = THIS_MODULE,
        .read = drv_read,
        .write = drv_write,

        .llseek = NULL, .open = NULL, .release = NULL
    };

    // Create driver class
    class = class_create(THIS_MODULE, DRIVER_NAME__);
    if(!IS_ERR(class)) {
        // Assign mode callback (to set device files under this driver to 0666)
        class->devnode = devnode_rwrwrw;
        printk(KERN_INFO DRIVER_NAME "Class registered: %s\n", class->name);
    } else {
        printk(KERN_INFO DRIVER_NAME "class_create error: %ld\n", PTR_ERR(class));
        return PTR_ERR(class);
    }

    // Get major number
    dev_alloc_rc = alloc_chrdev_region(&dev, CHAR_DRV_FIRST_MINOR, CHAR_DRV_C, driver_device_name);
    printk(KERN_INFO DRIVER_NAME "dev_t reg: %d\n", dev);

    // Abort if alloc_chrdev_region() fails
    if(dev_alloc_rc != 0) {
        printk(KERN_INFO DRIVER_NAME "Exiting with dev_alloc_rc: %d\n", dev_alloc_rc);
        return dev_alloc_rc;
    }

    // Create device file
    if(!IS_ERR(class) && dev_alloc_rc == 0) {
        printk(KERN_INFO DRIVER_NAME "Creating dev file. Class: %p Dev: %d Name: %s\n", class, MAJOR(dev), driver_device_name);
        device_file = device_create(class, NULL, dev, NULL, "%s", driver_device_name);
    }

    if(IS_ERR(device_file)) {
        printk(KERN_INFO DRIVER_NAME "device_create: %ld\n", PTR_ERR(device_file));
    } else {
        printk(KERN_INFO DRIVER_NAME "/dev/%s created\n", driver_device_name);
    }

    // Register chrdev with kernel
    cdev = cdev_alloc();
    cdev_init(cdev, &fops);
    cdev_add(cdev, dev, 1);

    return 0;
}

static void __exit __drv_exit(void) {
    if(device_file && !IS_ERR(device_file))
        device_destroy(class, dev);
    if(dev_alloc_rc == 0)
        unregister_chrdev_region(dev, CHAR_DRV_C);
    if(cdev)
        cdev_del(cdev);
    if(class && !IS_ERR(class))
        class_destroy(class);

    printk(KERN_INFO DRIVER_NAME "%s:%d Done\n", __func__, __LINE__);
}

module_init(__drv_init);
module_exit(__drv_exit);
MODULE_LICENSE("GPL");