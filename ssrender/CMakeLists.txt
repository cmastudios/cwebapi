add_executable(webrendertest src/main.cpp src/router.cpp src/server.cpp src/index.cpp src/roll.cpp src/errors.cpp)
function(add_html_template input_html output_hpp user_cpp)
    add_custom_command(
        OUTPUT ${output_hpp}
        COMMAND stdoutwrapper ${output_hpp} $<TARGET_FILE:webtemplate> ${CMAKE_CURRENT_SOURCE_DIR}/${input_html}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${input_html}
        VERBATIM
    )
    set_source_files_properties(${user_cpp} OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${output_hpp})
    target_include_directories(webrendertest PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
endfunction()
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/pages)
add_html_template(pages/roll.html pages/roll.hpp src/roll.cpp)
add_html_template(pages/index.html pages/index_tpl.hpp src/index.cpp)
add_html_template(pages/404.html pages/404_tpl.hpp src/errors.cpp)
target_link_libraries(webrendertest PUBLIC sqlite3)
