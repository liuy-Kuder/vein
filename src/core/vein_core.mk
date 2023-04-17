CSRCS += vein_bus.c
CSRCS += vein_device.c
CSRCS += vein_driver.c
CSRCS += vein_init.c

DEPPATH += --dep-path $(VEIN_DIR)/$(VEIN_DIR_NAME)/src/core
VPATH += :$(VEIN_DIR)/$(VEIN_DIR_NAME)/src/core

CFLAGS += "-I$(VEIN_DIR)/$(VEIN_DIR_NAME)/src/core"
