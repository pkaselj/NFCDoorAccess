project("Mailbox")

include_directories("include")

add_library(DataMailboxLib SHARED "include/DataMailbox.hpp" "src/DataMailbox.cpp")

target_include_directories(DataMailboxLib PUBLIC "${MailboxAPI_SOURCE_DIR}/include"
												 "${Watchdog_SOURCE_DIR}/include")

target_link_libraries(DataMailboxLib PUBLIC SimplifiedMailboxLib NulLoggerLib LoggerLib KernelLib WatchdogSettingsLib)