project("DatabaseTables")

include_directories("include/")


add_library(EmployeesTableLib SHARED "include/Tables.hpp" "src/EmployeesTable.cpp")

target_include_directories(EmployeesTableLib PUBLIC "${MailboxAPI_SOURCE_DIR}/include"
                                                  "${SQLite3_Database_SOURCE_DIR}/include"
                                                  "${Logger_SOURCE_DIR}/include"
                                                  "${Mailbox_SOURCE_DIR}/include"
                                                  "${Watchdog_SOURCE_DIR}/include")

target_link_libraries(EmployeesTableLib SQLite3DatabaseLib LoggerLib TimeLib)


add_library(KeypadPassTableLib SHARED "include/Tables.hpp" "src/KeypadPassTable.cpp")

target_include_directories(KeypadPassTableLib PUBLIC "${MailboxAPI_SOURCE_DIR}/include"
                                                  "${SQLite3_Database_SOURCE_DIR}/include"
                                                  "${Logger_SOURCE_DIR}/include"
                                                  "${Mailbox_SOURCE_DIR}/include"
                                                  "${Watchdog_SOURCE_DIR}/include")

target_link_libraries(KeypadPassTableLib SQLite3DatabaseLib LoggerLib TimeLib)


add_library(LogTableLib SHARED "include/Tables.hpp" "src/LogTable.cpp")

target_include_directories(LogTableLib PUBLIC "${MailboxAPI_SOURCE_DIR}/include"
                                                  "${SQLite3_Database_SOURCE_DIR}/include"
                                                  "${Logger_SOURCE_DIR}/include"
                                                  "${Mailbox_SOURCE_DIR}/include"
                                                  "${Watchdog_SOURCE_DIR}/include")

target_link_libraries(LogTableLib SQLite3DatabaseLib LoggerLib TimeLib)


add_library(RFIDCardTableLib SHARED "include/Tables.hpp" "src/RFIDCardTable.cpp")

target_include_directories(RFIDCardTableLib PUBLIC "${MailboxAPI_SOURCE_DIR}/include"
                                                  "${SQLite3_Database_SOURCE_DIR}/include"
                                                  "${Logger_SOURCE_DIR}/include"
                                                  "${Mailbox_SOURCE_DIR}/include"
                                                  "${Watchdog_SOURCE_DIR}/include")

target_link_libraries(RFIDCardTableLib SQLite3DatabaseLib LoggerLib TimeLib)




add_library(WebAPITableLib SHARED "include/Tables.hpp" "src/WebAPITable.cpp")

target_include_directories(WebAPITableLib PUBLIC "${MailboxAPI_SOURCE_DIR}/include"
                                                  "${SQLite3_Database_SOURCE_DIR}/include"
                                                  "${Logger_SOURCE_DIR}/include"
                                                  "${Mailbox_SOURCE_DIR}/include"
                                                  "${Watchdog_SOURCE_DIR}/include")

target_link_libraries(WebAPITableLib SQLite3DatabaseLib LoggerLib TimeLib)




add_library(CommandsTableLib SHARED "include/Tables.hpp" "src/CommandsTable.cpp")

target_include_directories(CommandsTableLib PUBLIC "${MailboxAPI_SOURCE_DIR}/include"
                                                  "${SQLite3_Database_SOURCE_DIR}/include"
                                                  "${Logger_SOURCE_DIR}/include"
                                                  "${Mailbox_SOURCE_DIR}/include"
                                                  "${Watchdog_SOURCE_DIR}/include")

target_link_libraries(CommandsTableLib SQLite3DatabaseLib LoggerLib TimeLib)