#include <linux/module.h> //Module related defines
#include <linux/fs.h> //chrdev_region and VFS structure definition
#include <linux/cdev.h> // cdev definition
#include <linux/device.h> // device file creation/destruction
#include <linux/uaccess.h> // user space Access
#include <linux/kdev_t.h> // To extract Major and Minor numbers
#include <linux/export.h> // for THIS_MODULE
#include <linux/kernel.h>
//#include <linux/container_of.h>
#include "nNodeDrv.h"

struct pcd_driver prvPcdDrv = {
	.TotNumDev = MAX_DEV,
	.pcdDev = {

		[0] = {
			.buffer = DEV_MEM_BUFFER_0,
			.serialNo = "DEVMEMBUF0",
			.size = DEV_MEM_SIZE_512,
			.perm = 0x01, //RDONLY
		},

		[1] = {
			.buffer = DEV_MEM_BUFFER_1,
			.serialNo = "DEVMEMBUF1",
			.size = DEV_MEM_SIZE_1024,
			.perm = 0x10 //WRONLY
		},

		[2] = {
			.buffer = DEV_MEM_BUFFER_2,
			.serialNo = "DEVMEMBUF2",
			.size = DEV_MEM_SIZE_512,
			.perm = 0x11 //RDWR
		},

		[3] = {
			.buffer = DEV_MEM_BUFFER_3,
			.serialNo = "DEVMEMBUF3",
			.size = DEV_MEM_SIZE_1024,
			.perm = 0x11 //RDWR
		}
	}
};




static ssize_t pcdDevRead (struct file *filep, char __user *buff, size_t count, loff_t *f_off) {
	struct pcd_device *prvPcdDev;

	prvPcdDev = (struct pcd_device *)filep->private_data;

	pr_info("%s: initial, read count = %ld, f_off = %lld\n", __func__, count, *f_off);

	
	if (count > (*f_off + prvPcdDev->size)) {
		count = prvPcdDev->size - *f_off;
	}

	if(!count) {
		pr_info("%s:Nothing to read\n",__func__);
		return 0;
	}

	if(copy_to_user (buff, prvPcdDev->buffer+*f_off, count)) {
		pr_err("%s:Reading from device failed\n",__func__);
		return EIO;
	}

	*f_off=count;

	pr_info("%s: At end, read count = %ld, f_off = %lld\n", __func__, count, *f_off);

	return count;
}

static ssize_t pcdDevWrite (struct file *filep, const char __user *buff, size_t count, loff_t *f_off) {

	struct pcd_device *prvPcdDev;

	prvPcdDev = (struct pcd_device *)filep->private_data;

	pr_info("%s: initial, write count = %ld, f_off = %lld\n", __func__, count, *f_off);

        if (count > (*f_off + prvPcdDev->size)){
                count = prvPcdDev->size - *f_off;
        }

	if (!count) {
		pr_err("%s:No space left\n",__func__);
		return ENOMEM;
	 }	

        if (copy_from_user (prvPcdDev->buffer+*f_off, buff,  count)) {
		pr_err("%s: writing to device failed\n",__func__);
	}
        *f_off=count;

        pr_info("%s: At end, write count = %ld, f_off = %lld\n", __func__, count, *f_off);

	return count;
}

int checkPermission(void) {
	return 0;
}

int pcdDevOpen (struct inode *devNode, struct file *filep) {

	struct pcd_device *prvPcdDev;
	int minor, ret = 0;

	pr_info("Dev open called\n");
	
	/*To find, which device's open called*/
	minor = MINOR(devNode->i_rdev);
	pr_info("%s:device PcdDev-%d is opened",__func__, minor);

	/*Fetch and save opened device structure.*/
	prvPcdDev = container_of(devNode->i_cdev, struct pcd_device, devCdev);

	/*Pass Private Structure to read and write functions*/
	filep->private_data = prvPcdDev;

	/*Check Permission*/
	ret = checkPermission();
	if(ret) {
		pr_err("%s:file operation not permitted\n",__func__);
	}

	return 0;
}

int pcdDevRelease (struct inode *devNode, struct file *filep) {

	pr_info("Dev release called\n");
	return 0;
}

struct file_operations pcdDevFops = {
	.read = pcdDevRead,
	.write = pcdDevWrite,
	.open = pcdDevOpen,
	.release = pcdDevRelease
};

static int __init my_module_init(void) {
	int ret = 0;
	int i   = 0;

	/* Allocate major and minor number dynamically */
	ret = alloc_chrdev_region (&prvPcdDrv.devNum, 0, MAX_DEV, "PcdDevRegion");
	if(ret) {
		pr_err("Failed to allocate Major:Minor number\n");
		goto out;
	}

	/* create device class: entry into /sys/class/<your-class-name>*/
	prvPcdDrv.pcdDevClass = class_create (THIS_MODULE, "pcdClass");
	if (IS_ERR(prvPcdDrv.pcdDevClass)) {
		pr_err("Failed to create class in /sys/class/%s", "pcdClass");
		ret = PTR_ERR(prvPcdDrv.pcdDevClass);
		goto unregister_chrdev;
	}

	for(i=0;i<MAX_DEV;i++) {
		/* initialise cdev structure with File operation structure.*/
		cdev_init (&(prvPcdDrv.pcdDev[i].devCdev), &pcdDevFops);
		prvPcdDrv.pcdDev[i].devCdev.owner=THIS_MODULE;
		
		/* Add cdev entry to VFS*/
		ret = cdev_add (&(prvPcdDrv.pcdDev[i].devCdev), prvPcdDrv.devNum, 1);
		if(ret) {
			pr_err("Failed to bind cdev with VFS\n");
			goto unreg_cdev;
		}

		/* Create uevent to entry in /dev/ */
		prvPcdDrv.pcdDevicePtr = device_create(prvPcdDrv.pcdDevClass, NULL, prvPcdDrv.devNum+i, NULL, "PcdDev-%d", i);
		if(IS_ERR(prvPcdDrv.pcdDevicePtr)) {
			pr_err("Device create failed\n");
			goto unreg_cdev;
		}
	}

	pr_info("Hello, Module is loaded with Major(%d):Minor(%d)\n", MAJOR(prvPcdDrv.devNum), MINOR(prvPcdDrv.devNum));
	return 0;

unreg_cdev:
	for (;i>=0;i--) {
		device_destroy(prvPcdDrv.pcdDevClass,  prvPcdDrv.devNum+i);
		cdev_del(&prvPcdDrv.pcdDev[i].devCdev);
	}

	class_destroy(prvPcdDrv.pcdDevClass);

unregister_chrdev:
	unregister_chrdev_region(prvPcdDrv.devNum, MAX_DEV);

out:
	return ret;
}

static void __exit my_module_exit(void) {
	int i = 0;

    /* Discard class and device from /dev */
	for(i=0;i<MAX_DEV;i++) {
		device_destroy(prvPcdDrv.pcdDevClass, prvPcdDrv.devNum+i);
		cdev_del (&(prvPcdDrv.pcdDev[i].devCdev));
	}

    class_destroy (prvPcdDrv.pcdDevClass);

	/* Deallocate major and minor numbers */
	unregister_chrdev_region(prvPcdDrv.devNum,MAX_DEV);
	
	pr_info("Bye, Module Discarded\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mitul Golani");
MODULE_DESCRIPTION("SAMPLE KERNEL MODULE");

