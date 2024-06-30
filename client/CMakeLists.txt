add_custom_target(client ALL COMMAND rm -rf build && npm run --prefix ${CMAKE_CURRENT_SOURCE_DIR} build && mv ${CMAKE_CURRENT_SOURCE_DIR}/build ./)
