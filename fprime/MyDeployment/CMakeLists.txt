#####
# 'MyDeployment' Deployment:
#
# This sets up the build for the 'MyDeployment' Application, including custom
# components. In addition, it imports FPrime.cmake, which includes the core F Prime components.
#
#####

###
# Basic Project Setup
###
cmake_minimum_required(VERSION 3.13)
cmake_policy(SET CMP0048 NEW)

###
# Zephyr glue
###
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
# Needed for loading the zephyr.cmake platform file from F Prime.
set(CMAKE_SYSTEM_NAME "Zephyr")
project(MyDeployment VERSION 1.0.0 LANGUAGES C CXX)

# zephyr_compile_options(-fcallgraph-info=su,da)

###
# F' Core Setup
# This includes all of the F prime core components, and imports the make-system.
###
# Needed for using patched fpp-to-cpp.
set(FPRIME_SKIP_TOOLS_VERSION_CHECK ON)

# Space saving testing.
# set(FPRIME_ENABLE_TEXT_LOGGERS OFF)

set(FPRIME_FRAMEWORK_PATH "${CMAKE_CURRENT_LIST_DIR}/../fprime" CACHE PATH "Location of F prime framework" FORCE)
set(FPRIME_PROJECT_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." CACHE PATH "Root path of F prime project" FORCE)
include("${FPRIME_FRAMEWORK_PATH}/cmake/FPrime.cmake")
# NOTE: register custom targets between these two lines
include("${FPRIME_FRAMEWORK_PATH}/cmake/FPrime-Code.cmake")


###
# Components and Topology
###
include("${FPRIME_PROJECT_ROOT}/project.cmake")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Top/")

###
# Final binary
###
target_sources(app PRIVATE Main.cpp)
target_link_libraries(app PRIVATE MyDeployment_Top)