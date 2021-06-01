cmake_minimum_required (VERSION 3.8)

project ("Startup")

add_executable(Startup "src/startup.cpp")

configure_file("run.sh" "run.sh" COPYONLY)

set(HardwareDaemonFilePath "../HardwareDaemon/HardwareDaemon")
set(HardwareDaemonTarget "HardwareDaemon.ln")

set(MainApplicationFilePath "../MainApplication/MainApplication")
set(MainApplicationTarget "MainApplication.ln")

set(DatabaseGatewayFilePath "../DatabaseGateway/DatabaseGateway")
set(DatabaseGatewayTarget "DatabaseGateway.ln")


add_custom_target(link_target_hd ALL
				  COMMAND ${CMAKE_COMMAND} -E create_symlink ${HardwareDaemonFilePath} ${HardwareDaemonTarget} )

add_custom_target(link_target_ma ALL
				  COMMAND ${CMAKE_COMMAND} -E create_symlink ${MainApplicationFilePath} ${MainApplicationTarget} )

add_custom_target(link_target_dbg ALL
				  COMMAND ${CMAKE_COMMAND} -E create_symlink ${DatabaseGatewayFilePath} ${DatabaseGatewayTarget} )

add_custom_target( install_links DEPENDS ${ExecutableTargets})

# Include directories
target_include_directories(Startup PUBLIC "${Settings_SOURCE_DIR}/include"
										  "${Watchdog_SOURCE_DIR}/include"
										  "${ProcessManager_SOURCE_DIR}/include"
										  "${Mailbox_SOURCE_DIR}/include")
# Linked libraries
target_link_libraries(Startup WatchdogServerLib DataMailboxLib SimplifiedMailboxLib ProcessManagerLib ProcessLib LoggerLib NulLoggerLib KernelLib TimeLib TimerLib UNIX_SignalHandlerLib rt pthread)

