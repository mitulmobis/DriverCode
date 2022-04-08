#include <linux/module.h> //Module related defines
#include <linux/fs.h> //chrdev_region and VFS structure definition
#include <linux/device.h> // device file creation/destruction
#include <linux/uaccess.h> // user space Access
#include <linux/kdev_t.h> // To extract Major and Minor numbers
#include <linux/export.h> // for THIS_MODULE
#include <linux/kernel.h>
#include "PlatformDefines.h"
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/mod_devicetable.h>

struct pcd_platform_driver pvtPltDrv;


static ssize_t pcdDevRead (struct file *filep, char __user *buff, size_t count, loff_t *f_off) {

    return 0;
}

static ssize_t pcdDevWrite (struct file *filep, const char __user *buff, size_t count, loff_t *f_off) {

    return 0;
}

int pcdDevOpen (struct inode *devNode, struct file *filep) {

    return 0;
}

int pcdDevRelease (struct inode *devNode, struct file *filep) {

    return 0;
}

struct file_operations pcdDevFops = {
	.read = pcdDevRead,
	.write = pcdDevWrite,
	.open = pcdDevOpen,
	.release = pcdDevRelease
};

static int pltDrv_probe (struct platform_device *pdev) {
    int ret = 0;
    
//     struct pcd_platform_device  *prvDevData;
//     struct platform_data        *deviceData;

//     /*1. Get the Platform Data*/
//     deviceData = (struct platform_data *)dev_get_platdata(&pdev->dev);
 
//     /*2. Dynamically allocate memory for device private data*/
//     prvDevData = kzalloc(sizeof(struct pcd_platform_device), GFP_KERNEL);

//     /*3. Dynamically allocate memory for the device buffer using size information from the platform data*/
//     prvDevData->buffer = kzalloc(sizeof(deviceData->size), GFP_KERNEL);

//     prvDevData->pdata.serialNo = deviceData->serialNo;
//     prvDevData->pdata.size = deviceData->size;
//     prvDevData->pdata.permission = deviceData->permission;

//     pr_info("%s: Device Name = %s", __func__, pdev->name);
//     pr_info("%s: Device Serial Number = %s",__func__, prvDevData->pdata.serialNo);
//     pr_info("%s: Device Serial Number = %d",__func__, prvDevData->pdata.size);
//     pr_info("%s: Device Serial Number = %x",__func__, prvDevData->pdata.permission);

//     /*4. Get the device number*/
//     prvDevData->devNum = pvtPltDrv.pcdBaseNum+pdev->id;

//     /*5. Do cdev_init and cdev_add*/
//     cdev_init(&prvDevData->pCdev, &pcdDevFops);

//     ret = cdev_add(&prvDevData->pCdev, prvDevData->devNum, 1);
//     if(ret) {
//         pr_err("%s:cdev_add failed\n",__func__);
//         cdev_del(&prvDevData->pCdev);
//         goto free_buf;
//     }
// pr_err("%s: cdev add successful\n", __func__);
// /*6. create device file for the detected platform device*/
//     pvtPltDrv.pcdDevice = device_create(pvtPltDrv.pcdDevClass, NULL, prvDevData->devNum, NULL, "myPlatformDev.%d", pdev->id);
//     if(IS_ERR(pvtPltDrv.pcdDevice)) {
//         pr_err("%s:Device creation failed\n",__func__);
//         ret=PTR_ERR(pvtPltDrv.pcdDevice);
//         goto cdev_del;
//     }

//     dev_set_drvdata(&pdev->dev, (void *)prvDevData);

//     pr_info("Probe is successful for Device:%s\n",pdev->name);

//     return 0;
// /*7. Error handling*/

// cdev_del:
//     cdev_del(&prvDevData->pCdev);

// free_buf:
//     kfree(prvDevData->buffer);
//     kfree(prvDevData);
pr_info("probe called\n");
    return ret;
}

static int pltDrv_remove (struct platform_device *pdev) {
    //struct pcd_platform_device *drvData = (struct pcd_platform_device*)dev_get_drvdata(&pdev->dev);

    //  /*1. Remove the device which created during device_create*/
    //  device_destroy(pvtPltDrv.pcdDevClass, pvtPltDrv.pcdBaseNum);

    // // /*2. Remove cdev entry from system */
    //  cdev_del (&drvData->pCdev);

    // /*free memory reserved*/
    // kfree(drvData->buffer);
    // kfree(drvData);

    pr_info("device remove called\n");
    return 0;
}

struct 	platform_device_id platform_device_id [] = {
        {.name="PCDDEVAx"},
        {.name="PCDDEVBx"},
        {.name="PCDDEVCx"},
        {}
};

struct platform_driver pcdPlatformDriver = {
    .probe = pltDrv_probe,
    .remove = pltDrv_remove,
    .id_table = platform_device_id,
    .driver = {
        .name = "PcdPlatformDevice",
        .owner = THIS_MODULE
    }
};

static int __init my_module_init(void) {
    int ret = 0;

    /*1. Dynamically allocate Major (Which driver) and Minor(which device is using driver) */
    ret = alloc_chrdev_region (&pvtPltDrv.pcdBaseNum, 0, MAX_DEVICE, "PlatformDevRegion");
    if (ret) {
        pr_err("%s:failed to allocate Major and Minor number\n", __func__);
        goto out;
    }

    /*2. Creating class in /sys/class/<your-class-name> */
    pvtPltDrv.pcdDevClass = class_create (THIS_MODULE, "pltDevClass");
    if(IS_ERR(pvtPltDrv.pcdDevClass)) {
        ret = PTR_ERR(pvtPltDrv.pcdDevClass);
        pr_err("failed create class under /sys/class/");
        goto unreg_chrdev;
    }

    ret = platform_driver_register (&pcdPlatformDriver);
    if(ret) {
        pr_err("Platform Driver not registered\n");
        goto unregister_class;
    }

    pr_info("pcdPlatformDriver module inserted\n");

    return ret;

unregister_class:
    class_destroy(pvtPltDrv.pcdDevClass);

unreg_chrdev:
    unregister_chrdev_region(pvtPltDrv.pcdBaseNum, MAX_DEVICE);

out:
    return ret;
}

static void __exit my_module_exit(void) {
    unregister_chrdev_region(pvtPltDrv.pcdBaseNum, MAX_DEVICE);

    class_destroy(pvtPltDrv.pcdDevClass);

    platform_driver_unregister(&pcdPlatformDriver);
	
	pr_info("Bye, Module Discarded\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mitul Golani");
MODULE_DESCRIPTION("SAMPLE KERNEL MODULE");

