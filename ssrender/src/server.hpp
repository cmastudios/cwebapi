#pragma once
#include <cstring>
#include <string>
#include <unordered_map>

void run_scgi_server(void);

class Connection
{
public:
    virtual int content_length() = 0;
    virtual int read(char * buffer, int size) = 0;
    virtual int write_status(int status) = 0;
    virtual int write(const char * buffer, int size) = 0;
    int write_string(const char * str) {
        return write(str, static_cast<int>(strlen(str)));
    }
    int write_string(const std::string & str) {
        return write_string(str.c_str());
    }
    std::string read_string(int size) {
        std::string s(size, ' ');
        int actual = read(s.data(), size);
        s.erase(s.begin() + actual, s.end());
        return s;
    }
    virtual const std::unordered_map<std::string, std::string> & request_headers() = 0;
};