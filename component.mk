# ESP-IDF component file for make based commands

COMPONENT_SRCDIRS := . \
                  src \
                  src/core \
                  src/lib \
                  src/log \

COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_SRCDIRS) .
