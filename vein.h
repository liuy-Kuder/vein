#ifndef  __VEIN_H_
#define  __VEIN_H_

#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

#include "src/core/vein_init.h"
#include "src/core/vein_list.h"
#include "src/core/vein_bus.h"
#include "src/core/vein_driver.h"
#include "src/core/vein_device.h"
#include "src/log/vein_log.h"

#if defined CONFIG_VEIN_CONF_SKIP
    #include "vein_kconfig.h"
#else
    #include "vein_conf.h"
#endif
/***************************
 * CURRENT VERSION OF CM
 ***************************/
#define VEIN_VERSION_MAJOR 1
#define VEIN_VERSION_MINOR 1
#define VEIN_VERSION_PATCH 0
#define VEIN_VERSION_INFO  "setup"

#define ARRAY_SIZE(array)	(sizeof(array) / sizeof((array)[0]))
#define VEIN_OK			 0
#define VEIN_ERR		-1

/**
 * Wrapper functions for VERSION macros
 */
static inline int vein_version_major(void)
{
    return VEIN_VERSION_MAJOR;
}

static inline int vein_version_minor(void)
{
    return VEIN_VERSION_MINOR;
}

static inline int vein_version_patch(void)
{
    return VEIN_VERSION_PATCH;
}

static inline const char *vein_version_info(void)
{
    return VEIN_VERSION_INFO;
}

#endif
