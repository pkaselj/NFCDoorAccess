project("ErrorCodes")

include_directories("include")

add_library(ErrorCodesLib STATIC "include/errorCodes.h" "include/errorTemplates.h")

set_target_properties(ErrorCodesLib PROPERTIES LINKER_LANGUAGE CXX )