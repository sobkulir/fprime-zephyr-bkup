add_compile_definitions(TGT_OS_TYPE_ZEPHYR)

set(FPRIME_USE_BAREMETAL_SCHEDULER OFF)

# Zephyr compiler options
include_directories(
    $<TARGET_PROPERTY:zephyr_interface,INTERFACE_INCLUDE_DIRECTORIES>
    )

include_directories(SYSTEM
    $<TARGET_PROPERTY:zephyr_interface,INTERFACE_SYSTEM_INCLUDE_DIRECTORIES>
    )

add_compile_definitions(
    $<TARGET_PROPERTY:zephyr_interface,INTERFACE_COMPILE_DEFINITIONS>
    )

add_compile_options(
    $<TARGET_PROPERTY:zephyr_interface,INTERFACE_COMPILE_OPTIONS>
    -fno-builtin
    -Wno-shadow -Wno-cast-align
    )


include_directories(SYSTEM "${CMAKE_CURRENT_LIST_DIR}/types")