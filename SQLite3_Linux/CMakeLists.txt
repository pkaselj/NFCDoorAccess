project("SQLite3_Linux")

include_directories("include")

add_library(SQLite3Lib SHARED "src/sqlite3.c" "include/sqlite3.h")

target_link_libraries(SQLite3Lib dl)