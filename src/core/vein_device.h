#ifndef __VEIN_DEVICE_H_
#define __VEIN_DEVICE_H_

#include "vein.h"

#ifdef __cplusplus
extern "C" {
#endif

struct device_base_info_t{
	char * driver_name;
	char * bus_name;
	char * device_name;
	uint16_t id;
};

struct vein_device_t
{
    struct hlist_node node;
    char * name;
    int16_t id;
    struct vein_bus_t * bus;
    /* device read */
    int16_t(*read)(struct vein_device_t * device, uint32_t offset, void * buf, uint32_t size);
    /* device write */
    int16_t(*write)(struct vein_device_t * device, uint32_t offset, void * buf, uint32_t size);
    /* device ioctl */
    int16_t(*ioctl)(struct vein_device_t * device, uint64_t cmd, ...);

#ifdef DEVICE_INIT_PAUSE_IF
    /* device suspend */
    int16_t(*suspend)(struct vein_device_t * device);
    /* device resume */
    int16_t(*resume)(struct vein_device_t * device);
#endif

#ifdef DEVICE_INIT_LOW_POWER_IF
     /* device shutdown */
    int16_t(*shutdown)(struct vein_device_t * device);
    /* device startup */
    int16_t(*startup)(struct vein_device_t * device);
#endif
    void * priv;
};

typedef int16_t(*vein_device_read_t)(struct vein_device_t * device, uint32_t offset, void * buf, uint32_t size);
typedef int16_t(*vein_device_write_t)(struct vein_device_t * device, uint32_t offset, void * buf, uint32_t size);
typedef int16_t(*vein_device_ioctl_t)(struct vein_device_t * device, uint64_t cmd, ...);
typedef int16_t(*vein_device_suspend_t)(struct vein_device_t * device);
typedef int16_t(*vein_device_resume_t)(struct vein_device_t * device);
typedef int16_t(*vein_device_shutdown_t)(struct vein_device_t * device);
typedef int16_t(*vein_device_startup_t)(struct vein_device_t * device);

char * vein_alloc_device_name(const char * name, uint16_t id);
int vein_free_device_name(char * name);
struct vein_device_t * vein_search_device(const char * name, uint16_t id);
uint16_t vein_get_register_device_num(void);
int vein_register_device(struct vein_device_t * dev);
int vein_unregister_device(struct vein_device_t * dev);
void vein_device_init_basic_if(struct vein_device_t* device, vein_device_read_t read, vein_device_write_t write, vein_device_ioctl_t ioctl);
void vein_device_init_pause_if(struct vein_device_t * device, vein_device_suspend_t suspend, vein_device_resume_t resume);
void vein_device_init_low_power_if(struct vein_device_t * device, vein_device_shutdown_t shutdown, vein_device_startup_t startup);
void vein_device_pure_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __DEVICE_H__ */

