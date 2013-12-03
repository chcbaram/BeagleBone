#include <linux/ioctl.h>

/* 
Look in Documentation/ioctl/ioctl-numbers.txt
to choose your non-conflicting number
*/
#define IOC_MAGIC 60

// IO doesn't read or write
#define IOC_TOGGLE _IO(IOC_MAGIC, 1)
// IO Read
#define IOC_SET _IOR(IOC_MAGIC, 2, int)
// IO Write
#define IOC_GET _IOW(IOC_MAGIC, 3, int)

// Can be used to make sure the commmand passed doesn't exceed this number
#define IOC_MAXNR 5
