CSRCS += strdup.c

DEPPATH += --dep-path $(VEIN_DIR)/$(VEIN_DIR_NAME)/src/lib
VPATH += :$(VEIN_DIR)/$(VEIN_DIR_NAME)/src/lib

CFLAGS += "-I$(VEIN_DIR)/$(VEIN_DIR_NAME)/src/lib"
