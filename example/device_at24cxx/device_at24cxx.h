#ifndef  __DEVICE_AT24CXX_H_
#define  __DEVICE_AT24CXX_H_

#include <stdint.h>
#include "vein.h"

struct at24cxx_device_priv_t
{        
	uint8_t addr;//芯片地址
    void (*wp_ctrl)(uint8_t cmd);
	uint16_t capacity;//容量
    uint16_t page;
    uint16_t page_size;
};

struct device_at24cxx_info_t{
  	struct device_base_info_t base_info;
    struct at24cxx_device_priv_t priv_info;
};

struct device_test_t{
	char * device_name;
	uint16_t id;
	uint8_t page;//页
	uint8_t page_size;//页大小
};

#define AT24CXX_IOCTL_WP_ON    0
#define AT24CXX_IOCTL_WP_OFF   1

struct vein_device_t *register_at24cxx_dev(struct device_at24cxx_info_t * table);
int16_t unregister_at24cxx_dev(char *name,uint16_t id);

struct vein_device_t *init_at24cxx_device(void);

#endif
