#ifndef __VEIN_DRIVER_H_
#define __VEIN_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "vein_list.h"

struct vein_driver_t
{
	struct vein_bus_t * bus;
	struct hlist_node node;
	char *name;
	int (*probe)(struct vein_driver_t * drv,void * table);
	int (*remove)(struct vein_driver_t * drv, char *node_name);
	int (*suspend)(struct vein_driver_t * dev);
	int (*resume)(struct vein_driver_t * dev);
};

struct vein_driver_t * vein_search_driver(const char * name);
struct vein_bus_t* vein_search_driver_bus(const char * driver_name, const char * bus_name);
int vein_register_driver(struct vein_driver_t * drv);
int vein_unregister_driver(struct vein_driver_t * drv);
void vein_driver_pure_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __DRIVER_H__ */
