if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(COVERAGE_COMPILER_FLAGS "-g -O0 --coverage -fprofile-arcs -ftest-coverage")
    set(COVERAGE_LINKER_FLAGS "--coverage")

    set(CMAKE_CXX_FLAGS_COVERAGE
            ${COVERAGE_COMPILER_FLAGS}
            CACHE STRING "Flags used by the C++ compiler during coverage builds."
            FORCE
    )
    set(CMAKE_C_FLAGS_COVERAGE
            ${COVERAGE_COMPILER_FLAGS}
            CACHE STRING "Flags used by the C compiler during coverage builds."
            FORCE
    )
    set(CMAKE_EXE_LINKER_FLAGS_COVERAGE
            ${COVERAGE_LINKER_FLAGS}
            CACHE STRING "Flags used for linking binaries during coverage builds."
            FORCE
    )
    set(CMAKE_SHARED_LINKER_FLAGS_COVERAGE
            ${COVERAGE_LINKER_FLAGS}
            CACHE STRING "Flags used by the shared libraries linker during coverage builds."
            FORCE
    )

    mark_as_advanced(
            CMAKE_CXX_FLAGS_COVERAGE
            CMAKE_C_FLAGS_COVERAGE
            CMAKE_EXE_LINKER_FLAGS_COVERAGE
            CMAKE_SHARED_LINKER_FLAGS_COVERAGE
    )

    find_program(GCOV gcov)
    find_program(LCOV lcov)
    find_program(GENHTML genhtml)

    if(NOT GCOV)
        message(FATAL_ERROR "gcov not found! Needed for coverage.")
    endif()

    if(NOT LCOV)
        message(FATAL_ERROR "lcov not found! Needed for coverage.")
    endif()

    if(NOT GENHTML)
        message(FATAL_ERROR "genhtml not found! Needed for coverage.")
    endif()

    set(COVERAGE_DIR "${CMAKE_BINARY_DIR}/coverage")
    set(COVERAGE_HTML_DIR "${COVERAGE_DIR}/html")
    set(COVERAGE_INFO_FILE "${COVERAGE_DIR}/coverage.info")

    function(setup_target_for_coverage)
        cmake_parse_arguments(ARG "" "NAME;EXECUTABLE" "DEPENDENCIES" ${ARGN})

        add_custom_target(${ARG_NAME}
                COMMAND ${LCOV} --directory . --zerocounters
                COMMAND $<TARGET_FILE:${ARG_EXECUTABLE}>
                COMMAND ${LCOV} --directory . --capture --output-file ${COVERAGE_INFO_FILE}
                COMMAND ${LCOV} --remove ${COVERAGE_INFO_FILE} '/usr/*' '*/tests/*' '*/third_party/*' --output-file ${COVERAGE_INFO_FILE}
                COMMAND ${GENHTML} -o ${COVERAGE_HTML_DIR} ${COVERAGE_INFO_FILE}
                COMMAND ${LCOV} --list ${COVERAGE_INFO_FILE}
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                DEPENDS ${ARG_DEPENDENCIES}
                COMMENT "Running coverage and generating report"
        )

        add_custom_command(TARGET ${ARG_NAME} POST_BUILD
                COMMAND ;
                COMMENT "Open ${COVERAGE_HTML_DIR}/index.html in your browser to view the coverage report."
        )
    endfunction()

    function(add_coverage_compile_options TARGET)
        target_compile_options(${TARGET} PRIVATE ${COVERAGE_COMPILER_FLAGS})
        target_link_libraries(${TARGET} PRIVATE ${COVERAGE_LINKER_FLAGS})
    endfunction()
else()
    message(WARNING "Code coverage not supported for compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()