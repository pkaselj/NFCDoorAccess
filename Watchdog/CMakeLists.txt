project("Watchdog")

include_directories("include")

add_library(WatchdogSettingsLib SHARED "include/WatchdogSettings.hpp" "src/WatchdogSettings.cpp")
target_include_directories(WatchdogSettingsLib PUBLIC "${Kernel_SOURCE_DIR}/include")

target_link_libraries(WatchdogSettingsLib KernelLib)

add_library(WatchdogServerLib SHARED "include/WatchdogServer.hpp" "src/WatchdogServer.cpp")

target_include_directories(WatchdogServerLib PUBLIC "${Mailbox_SOURCE_DIR}/include"
													"${Time_SOURCE_DIR}/include"
													"${UNIX_SignalHandler_SOURCE_DIR}/include"
													"${ProcessManager_SOURCE_DIR}/include"
													"${SharedMemory_SOURCE_DIR}/include")

target_link_libraries(WatchdogServerLib LoggerLib NulLoggerLib TimerLib TimeLib DataMailboxLib SimplifiedMailboxLib WatchdogSettingsLib UNIX_SignalHandlerLib ProcessManagerLib SharedMemoryLib pthread)



add_library(WatchdogClientLib SHARED "include/WatchdogClient.hpp" "src/WatchdogClient.cpp")

target_include_directories(WatchdogClientLib PUBLIC "${Logger_SOURCE_DIR}/include"
													"${Time_SOURCE_DIR}/include"
													"${Mailbox_SOURCE_DIR}/include"
													"${SharedMemory_SOURCE_DIR}/include")

target_link_libraries(WatchdogClientLib LoggerLib NulLoggerLib TimerLib TimeLib DataMailboxLib WatchdogSettingsLib SharedMemoryLib pthread)