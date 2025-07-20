cmake_minimum_required(VERSION 3.10)
project(CoverageDemo)

# 自动开启 coverage（仅 Debug 模式）
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(STATUS "Enabling coverage instrumentation")
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
    set(ENABLE_COVERAGE TRUE)
else()
    set(ENABLE_COVERAGE FALSE)
endif()

enable_testing()

# 添加主库
add_library(mycode src/add.cpp)
target_include_directories(mycode PUBLIC ${CMAKE_SOURCE_DIR}/include)

# 添加测试
add_executable(test_add test/test_add.cpp)
target_link_libraries(test_add mycode gcov)
add_test(NAME test_add COMMAND test_add)

# 覆盖率报告自动生成
if(ENABLE_COVERAGE)
    find_program(LCOV_EXEC lcov)
    find_program(GENHTML_EXEC genhtml)

    add_custom_command(TARGET test_add POST_BUILD
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        COMMAND ${LCOV_EXEC} --directory . --capture --output-file coverage.info
        COMMAND ${LCOV_EXEC} --remove coverage.info '/usr/*' '*test*' --output-file coverage_filtered.info
        COMMAND ${GENHTML_EXEC} coverage_filtered.info --output-directory coverage_html
        COMMENT "Running test and generating coverage HTML report"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()
