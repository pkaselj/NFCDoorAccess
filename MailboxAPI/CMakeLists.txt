project("MailboxAPI")

include_directories("include")

# Mailbox Reference
add_library(MailboxReferenceLib SHARED "include/MailboxReference.hpp" "src/MailboxReference.cpp")
target_include_directories(MailboxReferenceLib PUBLIC "${Logger_SOURCE_DIR}/include"
													  "${Kernel_SOURCE_DIR}/include")
target_link_libraries(MailboxReferenceLib LoggerLib NulLoggerLib KernelLib rt)

# Mailbox
add_library(MailboxLib SHARED "include/mailbox.hpp" "src/mailbox.cpp")
target_include_directories(MailboxLib PUBLIC "${Time_SOURCE_DIR}/include")
target_link_libraries(MailboxLib TimeLib NulLoggerLib MailboxReferenceLib rt)

# Simplified Mailbox
add_library(SimplifiedMailboxLib SHARED "include/SimplifiedMailbox.hpp" "include/SimplifiedMailboxEnums.hpp" "src/SimplifiedMailbox.cpp")
target_include_directories(SimplifiedMailboxLib PUBLIC "${Time_SOURCE_DIR}/include"
													   "${MailboxAutomaton_SOURCE_DIR}/include")
target_link_libraries(SimplifiedMailboxLib TimeLib SimplifiedMailboxAutomatonLib MailboxLib NulLoggerLib rt)