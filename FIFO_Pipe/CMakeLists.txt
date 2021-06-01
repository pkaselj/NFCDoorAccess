project("FIFO_Pipe")

include_directories("include")

add_library(FIFO_PipeLib SHARED "include/pipe.hpp" "src/pipe.cpp")
target_include_directories(FIFO_PipeLib PUBLIC "${Kernel_SOURCE_DIR}/include"
											   "${Logger_SOURCE_DIR}/include"
											   "${ErrorCodes_SOURCE_DIR}/include")

target_link_libraries(FIFO_PipeLib KernelLib NulLoggerLib ErrorCodesLib)
