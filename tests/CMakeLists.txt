file(GLOB EXAMPLE_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/test-*.cpp")

foreach(EXAMPLE_FILE ${EXAMPLE_FILES})
    get_filename_component(EXAMPLE_FILE_NAME ${EXAMPLE_FILE} NAME_WE)

    add_executable(
        ${EXAMPLE_FILE_NAME}
        ${EXAMPLE_FILE}
    )

    target_link_libraries(
        ${EXAMPLE_FILE_NAME}
        PRIVATE
        hashlib
    )

    target_compile_definitions(
        ${EXAMPLE_FILE_NAME}
        PRIVATE
        -DHASHLIB_TEST_DIR="${CMAKE_CURRENT_LIST_DIR}"
    )
endforeach()