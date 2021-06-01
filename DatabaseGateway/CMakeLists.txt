project("DatabaseGateway")

include_directories("include")

add_executable(DatabaseGateway "src/DatabaseGateway.cpp")
target_include_directories(DatabaseGateway PUBLIC "${Mailbox_SOURCE_DIR}/include"
                                                  "${UNIX_SignalHandler_SOURCE_DIR}/include"
                                                  "${Watchdog_SOURCE_DIR}/include")

target_link_libraries(DatabaseGateway DatabaseRequestLib WatchdogClientLib DataMailboxLib DatabaseObjectLib UNIX_SignalHandlerLib)

add_library(DatabaseObjectLib SHARED "include/DatabaseObject.hpp" "include/ValidationUtils.hpp" "src/DatabaseObject.cpp")

target_include_directories(DatabaseObjectLib PUBLIC "${MailboxAPI_SOURCE_DIR}/include"
                                                  "${SQLite3_Database_SOURCE_DIR}/include"
                                                  "${Logger_SOURCE_DIR}/include"
                                                  "${Mailbox_SOURCE_DIR}/include"
                                                  "${Watchdog_SOURCE_DIR}/include"
                                                  "${DatabaseTables_SOURCE_DIR}/include")

target_link_libraries(DatabaseObjectLib EmployeesTableLib KeypadPassTableLib WebAPITableLib CommandsTableLib RFIDCardTableLib LogTableLib SQLite3DatabaseLib LoggerLib TimeLib)


add_library(DatabaseRequestLib SHARED "include/DatabaseRequest.hpp" "include/ValidationUtils.hpp" "src/DatabaseRequest.cpp")

target_include_directories(DatabaseRequestLib PUBLIC "${Logger_SOURCE_DIR}/include"
                                                     "${Mailbox_SOURCE_DIR}/include")

target_link_libraries(DatabaseRequestLib DatabaseObjectLib LoggerLib TimeLib)
