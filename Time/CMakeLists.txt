cmake_minimum_required (VERSION 3.8)

project("Time")

include_directories("include")

add_library(TimeLib SHARED "include/Time.hpp" "include/Timezones.hpp" "src/Time.cpp" )
target_link_libraries(TimeLib PUBLIC rt)

add_library(TimerLib SHARED "include/Timer.hpp" "src/Timer.cpp" )

target_include_directories(TimerLib PUBLIC "${Kernel_SOURCE_DIR}/include"
                                           "${UNIX_SignalHandler_SOURCE_DIR}/include"
                                           "${Logger_SOURCE_DIR}/include"
                                           "${Settings_SOURCE_DIR}/include")

target_link_libraries(TimerLib PUBLIC UNIX_SignalHandlerLib LoggerLib NulLoggerLib rt)

