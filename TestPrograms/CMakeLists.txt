project("TestPrograms")

add_executable(Logger_wSizeGuard_Test "functionalityTests/Logger_wSizeGuard_Test.cpp")
target_include_directories(Logger_wSizeGuard_Test PUBLIC "${Logger_SOURCE_DIR}/include")
target_link_libraries(Logger_wSizeGuard_Test LoggerLib)

add_executable(LibFreeFareTest "functionalityTests/LibFreeFareTest.cpp")
#target_include_directories(LibFreeFareTest )
target_link_libraries(LibFreeFareTest nfc freefare)

add_executable(DatabaseInsertProfilingTest "functionalityTests/DatabaseInsertProfilingTest.cpp")
target_include_directories(DatabaseInsertProfilingTest PUBLIC "${SQLite3_Database_SOURCE_DIR}/include"
															  "${Time_SOURCE_DIR}/include")
target_link_libraries(DatabaseInsertProfilingTest SQLite3DatabaseLib TimeLib)

add_executable(DateTimeTest "functionalityTests/DateTimeTest.cpp")
target_include_directories(DateTimeTest PUBLIC "${Time_SOURCE_DIR}/include")
target_link_libraries(DateTimeTest TimeLib rt)

add_executable(MailboxReferenceReconstruction "functionalityTests/MailboxReferenceReconstruction.cpp")
target_include_directories(MailboxReferenceReconstruction PUBLIC "${Mailbox_SOURCE_DIR}/include")
target_link_libraries(MailboxReferenceReconstruction DataMailboxLib)

add_executable(TablesTest "functionalityTests/TablesTest.cpp")
target_include_directories(TablesTest PUBLIC "${DatabaseTables_SOURCE_DIR}/include")
target_link_libraries(TablesTest LogTableLib)


add_executable(SignalHandlingTest_Flag "functionalityTests/SignalHandlingTest_Flag.cpp")
target_include_directories(SignalHandlingTest_Flag PUBLIC "${UNIX_SignalHandler_SOURCE_DIR}/include")
target_link_libraries(SignalHandlingTest_Flag UNIX_SignalHandlerLib)

add_executable(DataMailboxTest "functionalityTests/DataMailboxTest.cpp")
target_include_directories(DataMailboxTest PUBLIC "${Logger_SOURCE_DIR}/include"
										  "${Mailbox_SOURCE_DIR}/include")
target_link_libraries(DataMailboxTest LoggerLib DataMailboxLib pthread)

add_executable(PN532_Test "functionalityTests/PN532_test.cpp")
target_include_directories(PN532_Test PUBLIC "${PN532_NFC_Driver_SOURCE_DIR}/include")
target_link_libraries(PN532_Test PN532_NFC_Lib)

add_executable(HardwareUtilsTest "functionalityTests/HardwareUtilsTest.cpp")
target_include_directories(HardwareUtilsTest PUBLIC "${IndicatorController_SOURCE_DIR}/include"
													"${Mailbox_SOURCE_DIR}/include"
													"${MailboxAPI_SOURCE_DIR}/include"
													"${Time_SOURCE_DIR}/include"
													"${Watchdog_SOURCE_DIR}/include")
target_link_libraries(HardwareUtilsTest IndicatorControllerLib TimeLib TimerLib WatchdogSettingsLib pigpio pthread)

add_executable(TimerTest "functionalityTests/TimerTest.cpp")

target_include_directories(TimerTest PUBLIC "${Time_SOURCE_DIR}/include")
target_link_libraries(TimerTest TimerLib TimeLib LoggerLib NulLoggerLib pthread UNIX_SignalHandlerLib KernelLib rt)

add_executable(WatchdogServerTest_wProcessManager "functionalityTests/WatchdogServerTest_wProcessManager.cpp")
target_include_directories(WatchdogServerTest_wProcessManager PUBLIC "${Mailbox_SOURCE_DIR}/include"
													 "${Watchdog_SOURCE_DIR}/include"
													 "${Logger_SOURCE_DIR}/include"
													 "${UNIX_SignalHandler_SOURCE_DIR}/include"
													 "${ProcessManager_SOURCE_DIR}/include")

target_link_libraries(WatchdogServerTest_wProcessManager ProcessManagerLib WatchdogServerLib DataMailboxLib LoggerLib LoggerToStdoutLib)

add_executable(WatchdogServerTest_wProcessManager_Client "functionalityTests/WatchdogServerTest_wProcessManager_Client.cpp")
target_include_directories(WatchdogServerTest_wProcessManager_Client PUBLIC "${Mailbox_SOURCE_DIR}/include"
													 "${Watchdog_SOURCE_DIR}/include"
													 "${Logger_SOURCE_DIR}/include"
													 "${UNIX_SignalHandler_SOURCE_DIR}/include"
													 "${ProcessManager_SOURCE_DIR}/include")

target_link_libraries(WatchdogServerTest_wProcessManager_Client WatchdogClientLib DataMailboxLib LoggerLib LoggerToStdoutLib)


add_executable(WatchdogServerTest_wProcessManager_ClientWorking "functionalityTests/WatchdogServerTest_wProcessManager_ClientWorking.cpp")
target_include_directories(WatchdogServerTest_wProcessManager_ClientWorking PUBLIC "${Mailbox_SOURCE_DIR}/include"
													 "${Watchdog_SOURCE_DIR}/include"
													 "${Logger_SOURCE_DIR}/include"
													 "${UNIX_SignalHandler_SOURCE_DIR}/include"
													 "${ProcessManager_SOURCE_DIR}/include")

target_link_libraries(WatchdogServerTest_wProcessManager_ClientWorking WatchdogClientLib DataMailboxLib LoggerLib LoggerToStdoutLib)

add_executable(WatchdogServerTest "functionalityTests/WatchdogServerTest.cpp")
target_include_directories(WatchdogServerTest PUBLIC "${Mailbox_SOURCE_DIR}/include"
													 "${Watchdog_SOURCE_DIR}/include"
													 "${Logger_SOURCE_DIR}/include")

target_link_libraries(WatchdogServerTest DataMailboxLib WatchdogServerLib LoggerLib LoggerToStdoutLib)

#[[
add_executable(SimplifiedMailboxTest "functionalityTests/SimplifiedMailboxTest.cpp")
target_include_directories(SimplifiedMailboxTest PUBLIC "${Logger_SOURCE_DIR}/include"
										  "${MailboxAPI_SOURCE_DIR}/include"
										  "${Time_SOURCE_DIR}/include")
target_link_libraries(SimplifiedMailboxTest LoggerLib SimplifiedMailboxLib pthread TimeLib)

add_executable(SimplifiedMailboxTest_R "functionalityTests/SimplifiedMailboxTest_Receiver.cpp")

target_include_directories(SimplifiedMailboxTest_R PUBLIC "${Logger_SOURCE_DIR}/include"
										  "${MailboxAPI_SOURCE_DIR}/include")

target_link_libraries(SimplifiedMailboxTest_R LoggerLib SimplifiedMailboxLib pthread)

add_executable(SimplifiedMailboxTest_S "functionalityTests/SimplifiedMailboxTest_Sender.cpp")

target_include_directories(SimplifiedMailboxTest_S PUBLIC "${Logger_SOURCE_DIR}/include"
										  "${MailboxAPI_SOURCE_DIR}/include")

target_link_libraries(SimplifiedMailboxTest_S LoggerLib SimplifiedMailboxLib pthread)




add_executable(ProcessManagerDestructorDoubleFree "functionalityTests/ProcessManagerDestructorDoubleFree.cpp")
target_include_directories(ProcessManagerDestructorDoubleFree PUBLIC "${Logger_SOURCE_DIR}/include"
													 "${ProcessManager_SOURCE_DIR}/include")

target_link_libraries(ProcessManagerDestructorDoubleFree ProcessManagerLib ProcessLib LoggerLib)

add_executable(PipeNonblockingTest "functionalityTests/PipeNonblockingTest.cpp")
target_include_directories(PipeNonblockingTest PUBLIC "${Logger_SOURCE_DIR}/include"
																	 "${FIFO_Pipe_SOURCE_DIR}/include")

target_link_libraries(PipeNonblockingTest LoggerLib LoggerToStdoutLib FIFO_PipeLib)

]]