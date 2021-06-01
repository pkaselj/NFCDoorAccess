project("SQLite3_Database" CXX)

include_directories("include")

 add_library(StatementLib STATIC "include/Statement.hpp")

 set_target_properties(StatementLib PROPERTIES LINKER_LANGUAGE CXX )

 target_include_directories(StatementLib PUBLIC "${SQLite3_Linux_SOURCE_DIR}/include"
											    "${Kernel_SOURCE_DIR}/include"
											    "${Logger_SOURCE_DIR}/include")

target_link_libraries(StatementLib SQLite3Lib KernelLib NulLoggerLib)



add_library(SQLite3DatabaseLib SHARED "include/Database.hpp" "src/Database.cpp")
target_include_directories(SQLite3DatabaseLib PUBLIC "${Logger_SOURCE_DIR}/include")
target_link_libraries(SQLite3DatabaseLib StatementLib)