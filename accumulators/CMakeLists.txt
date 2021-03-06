# 
# alps-accumulators cmake configuration script.
# 

cmake_minimum_required (VERSION 2.8.12)

# project name
project (alps-accumulators CXX)
message ("\nALPSCore module : ${PROJECT_NAME}\n")

# add common modules from ../common/cmake
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../common/cmake)

# add common definitions, if being configured independently
if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  include(ALPSCommonModuleDefinitions)
  include(ALPSEnableMPI)
endif()

gen_documentation()

if (DocumentationOnly)
  return()
endif()

add_this_package(accumulator accumulators wrapper_set)

add_boost()
add_hdf5()
add_alps_package(alps-utilities alps-hdf5)
add_testing()
gen_pkg_config()
gen_cfg_module()
