#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>
#include "PlatformDefines.h"

struct platform_data pltData_0 = {
    .size = 524,
    .serialNo = "ABCD001",
    .permission = 0x11
};

struct platform_data pltData_1 = {
    .size = 1024,
    .serialNo = "ABCD002",
    .permission = 0x11
};

struct platform_data pltData_2 = {
    .size = 2048,
    .serialNo = "ABCD003",
    .permission = 0x11
};

struct platform_data pltData_3 = {
    .size = 524,
    .serialNo = "ABCD004",
    .permission = 0x11
};

void platformRelease (struct device *dev){
    pr_info("Device Released\n");
}

struct platform_device prvPltDev_0 = {
    .name = "PCDDEVAx",
    .id = 0,
    .dev = {
        .release = platformRelease,
        .platform_data = &pltData_0
    }
};

struct platform_device prvPltDev_1 = {
    .name = "PCDDEVBx",
    .id = 1,
    .dev = {
        .release = platformRelease,
        .platform_data = &pltData_1
    }
};

struct platform_device prvPltDev_2 = {
    .name = "PCDDEVCx",
    .id = 1,
    .dev = {
        .release = platformRelease,
        .platform_data = &pltData_2
    }
};
static int __init pcdDevInit(void) {
    int ret=0;
    pr_info("Platform Device init called\n");
    ret = platform_device_register(&prvPltDev_0);
    if(ret) {
        pr_err("Platform Driver registration failed %s", "pcdPlatforDev-0");
        platform_device_unregister(&prvPltDev_0);
        return ret;
    }
    ret = platform_device_register(&prvPltDev_1);
    if(ret) {
        pr_err("Platform Driver registration failed %s", "pcdPlatforDev-0");
        platform_device_unregister(&prvPltDev_0);
        return ret;
    }

    return ret;
}

static void __exit pcdDevExit(void) {
    pr_info("Platform Device exit called");
    platform_device_unregister(&prvPltDev_0);
    platform_device_unregister(&prvPltDev_1);
}

module_init(pcdDevInit);
module_exit(pcdDevExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mitul Golani");
MODULE_DESCRIPTION("Sample Platform Device Add");
