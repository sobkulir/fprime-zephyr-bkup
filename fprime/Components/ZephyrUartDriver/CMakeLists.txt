####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
	"${FPRIME_FRAMEWORK_PATH}/Drv/ByteStreamDriverModel/ByteStreamDriverModel.fpp"
	"${CMAKE_CURRENT_LIST_DIR}/ZephyrUartDriver.cpp"
)

# Necessary shared helpers
set(MOD_DEPS
	"Fw/Logger"
	"Drv/ByteStreamDriverModel"
)

register_fprime_module()

