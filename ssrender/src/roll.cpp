#include "roll.hpp"
#include <sstream>
#include <vector>
void roll(Connection & conn)
{
    std::vector<std::string> result;
    if (conn.request_headers().at("REQUEST_METHOD") == "POST")
    {
        result.push_back("Payload: " + conn.read_string(std::stoi(conn.request_headers().at("CONTENT_LENGTH"))));
    }
    std::ostringstream out;
#include "pages/roll.hpp"
    conn.write_status(200);
    conn.write_string("Content-Type: text/html\r\n\r\n");
    conn.write_string(out.str());
}