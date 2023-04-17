cmake_minimum_required(VERSION 3.16)

if(NOT ESP_PLATFORM)
  project(vein HOMEPAGE_URL https://github.com/liuy-Kuder/vein.git)
endif()
set(VEIN_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
if(ESP_PLATFORM)
  include(${CMAKE_CURRENT_LIST_DIR}/env_support/cmake/esp.cmake)
endif()
