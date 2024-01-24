project(parser)

set(parser-source-files
    "${PROJECT_SOURCE_DIR}/grammar.cpp"
    "${PROJECT_SOURCE_DIR}/util.cpp"
    "${PROJECT_SOURCE_DIR}/earley_parser.cpp"
    "${PROJECT_SOURCE_DIR}/lr_parser.cpp"
    CACHE INTERNAL "")

set(parser-include-files
    "${Parser_SOURCE_DIR}/include/grammar.hpp"
    "${Parser_SOURCE_DIR}/include/util.hpp"
    "${Parser_SOURCE_DIR}/include/earley_parser.hpp"
    "${Parser_SOURCE_DIR}/include/lr_parser.hpp"
    CACHE INTERNAL "")

add_library(
    parser SHARED ${parser-source-files} ${parser-include-files}
)

add_executable(run "${PROJECT_SOURCE_DIR}/main.cpp")
target_link_libraries(run parser)
