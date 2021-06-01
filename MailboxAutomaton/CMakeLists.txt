project("MailboxAutomaton")

include_directories("include")

 add_library(SimplifiedMailboxAutomatonLib SHARED "src/MailboxAutomaton.cpp" "include/MailboxAutomaton.hpp")
 target_include_directories(SimplifiedMailboxAutomatonLib PUBLIC	"${MailboxAPI_SOURCE_DIR}/include"
																	"${MealyAutomaton_SOURCE_DIR}/include")

 target_link_libraries(SimplifiedMailboxAutomatonLib MealyAutomatonLib)