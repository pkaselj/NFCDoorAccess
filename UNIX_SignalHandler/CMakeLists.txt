cmake_minimum_required (VERSION 3.8)

project("UNIX_SignalHandler")

include_directories("include")

add_library(UNIX_SignalHandlerLib "src/UNIX_SignalHandler.cpp" "include/UNIX_SignalHandler.hpp")

target_include_directories(UNIX_SignalHandlerLib PUBLIC "${Kernel_SOURCE_DIR}/include"
                                                        "${Logger_SOURCE_DIR}/include"
                                                        "${Settings_SOURCE_DIR}/include")

target_link_libraries(UNIX_SignalHandlerLib KernelLib LoggerLib pthread)