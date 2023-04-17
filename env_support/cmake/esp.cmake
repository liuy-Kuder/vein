cmake_minimum_required(VERSION 3.16)

file(GLOB_RECURSE SOURCES ${VEIN_ROOT_DIR}/src/*.c )

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS ${VEIN_ROOT_DIR}
                        ${VEIN_ROOT_DIR}/src ${VEIN_ROOT_DIR}/../)

target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLV_CONF_INCLUDE_SIMPLE")

if(CONFIG_VEIN_ATTRIBUTE_FAST_MEM_USE_IRAM)
  target_compile_definitions(${COMPONENT_LIB}
                              PUBLIC "-DLV_ATTRIBUTE_FAST_MEM=IRAM_ATTR")
endif()

idf_build_set_property(COMPILE_OPTIONS "-Wno-error" APPEND)
