cmake_minimum_required(VERSION 3.12)
project(hid C)

# find linux kernel dir
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
exec_program(uname ARGS -r OUTPUT_VARIABLE CMAKE_HOST_SYSTEM_VERSION)
set(KERNEL_INCLUDE "/home/mumumusuc/raspberry/linux")
#set(KERNEL_INCLUDE "/usr/src/linux-headers-${CMAKE_HOST_SYSTEM_VERSION}")
# include kernel dir
include_directories(
		"${KERNEL_INCLUDE}/include"
		"${KERNEL_INCLUDE}/mm"
)
add_definitions(-D__KERNEL__=1)
add_definitions(-DKBUILD_MODNAME=1)
add_definitions(-DCONFIG_FB_DEFERRED_IO=1)
add_definitions(-DCONFIG_HZ=1)
add_definitions(-DCONFIG_I2C=1)

aux_source_directory(. SRC_FILES)
add_executable(${PROJECT_NAME} ${SRC_FILES})