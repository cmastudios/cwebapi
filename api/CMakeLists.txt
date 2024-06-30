add_executable(webapitest "main.cpp")
target_link_libraries(webapitest PUBLIC Crow::Crow sqlite3)
