#ifndef PLATFORM_DEFINES_H_
#define PLATFORM_DEFINES_H_

#include <linux/cdev.h> // cdev definition

#define MAX_DEVICE 4

struct platform_data {
    int     size;
    char    *serialNo;
    int     permission;
};

struct pcd_platform_device {
    struct  platform_data   pdata;
    char                    *buffer;
    dev_t                   devNum;
    struct  cdev            pCdev;
};

struct pcd_platform_driver {
    int             totalDevice;
    dev_t           pcdBaseNum;
    struct  cdev    pcdCdev;
    struct  class   *pcdDevClass;
    struct  device  *pcdDevice;
};

#endif //PLATFORM_DEFINES_H_
