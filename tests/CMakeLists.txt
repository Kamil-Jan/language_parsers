set(test-source-files
    "${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_util.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_grammar.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_earley_parser.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_lr_parser.cpp"
    CACHE INTERNAL "")

add_executable(test_main ${test-source-files})

target_link_libraries(test_main GTest::gtest Threads::Threads parser)
target_include_directories(test_main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
