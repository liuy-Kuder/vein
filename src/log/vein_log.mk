CSRCS += vein_log.c

DEPPATH += --dep-path $(VEIN_DIR)/$(VEIN_DIR_NAME)/src/log
VPATH += :$(VEIN_DIR)/$(VEIN_DIR_NAME)/src/log

CFLAGS += "-I$(VEIN_DIR)/$(VEIN_DIR_NAME)/src/log"
