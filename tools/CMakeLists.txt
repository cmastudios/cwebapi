# Find Flex and Bison
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

# Generate lexer and parser
FLEX_TARGET(WEB_TEMPLATE webtemplate.l ${CMAKE_CURRENT_BINARY_DIR}/webtemplate.cpp)

add_executable(webtemplate ${FLEX_WEB_TEMPLATE_OUTPUTS})
add_executable(stdoutwrapper stdoutwrapper.c)