project("SharedMemory")

include_directories("include")

add_library(SharedMemoryLib STATIC "include/SharedMemory.hpp")

target_include_directories(SharedMemoryLib PUBLIC "${Logger_SOURCE_DIR}/include"
												  "${Kernel_SOURCE_DIR}/include")

target_link_libraries(SharedMemoryLib NulLoggerLib KernelLib)

set_target_properties(SharedMemoryLib PROPERTIES LINKER_LANGUAGE CXX)