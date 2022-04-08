
#ifndef NODE_DRV_H_
#define NODE_DRV_H_

#define MAX_DEV 4

#define DEV_MEM_SIZE_512  512
#define DEV_MEM_SIZE_1024 1024

char DEV_MEM_BUFFER_0[DEV_MEM_SIZE_512];
char DEV_MEM_BUFFER_1[DEV_MEM_SIZE_1024];
char DEV_MEM_BUFFER_2[DEV_MEM_SIZE_512];
char DEV_MEM_BUFFER_3[DEV_MEM_SIZE_1024];



struct pcd_device {
	char 			*buffer;
	char 			*serialNo;
	int 			size;
	struct 	cdev 	devCdev;
	int 			perm;
};


struct pcd_driver {
	int 				TotNumDev;
	struct 	pcd_device	pcdDev[MAX_DEV];
	struct 	class	 	*pcdDevClass;
	struct 	device 		*pcdDevicePtr;
	dev_t 				devNum;
};

#endif // NODE_DRV_H_