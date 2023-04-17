#ifndef __VEIN_BUS_H__
#define __VEIN_BUS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "vein_list.h"
#include <stdint.h>

enum vein_bus_type_t {
	VEIN_BUS_TYPE_DIR, //路径
	VEIN_BUS_TYPE_REG, //节点
};

struct vein_bus_t
{
	/* bus name */
	char * name;
	/* bus type DIR or REG */
	enum vein_bus_type_t type;
	/* bus's parent */
	struct vein_bus_t* parent;
	/* bus's entry */
	struct list_head entry;
	/* bus's children */
	struct list_head children;
	/* private data */
	void * priv;
};

struct vein_bus_t* vein_bus_get_root(void);
struct vein_bus_t* vein_bus_search(struct vein_bus_t* parent, const char * name);
struct vein_bus_t* vein_bus_search_directory_with_create(struct vein_bus_t* parent, const char * name);
struct vein_bus_t* vein_bus_alloc_directory(const char * name);
struct vein_bus_t* vein_bus_alloc_regular(const char * name, void * priv);
int vein_bus_free(struct vein_bus_t* bus);
int vein_bus_add(struct vein_bus_t* parent, struct vein_bus_t* bus);
int vein_bus_remove(struct vein_bus_t* parent, struct vein_bus_t* bus);
int vein_bus_add_directory(struct vein_bus_t* parent, const char * name);
int vein_bus_add_regular(struct vein_bus_t* parent, const char * name, void * priv);
int vein_bus_remove_self(struct vein_bus_t* bus);
unsigned int shash(const char * s);

#ifdef __cplusplus
}
#endif

#endif /* __bus_H__ */
