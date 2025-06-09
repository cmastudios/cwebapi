#include "index.hpp"
#include <vector>
#include <string>
#include <sstream>

void index(Connection & conn)
{
    std::ostringstream out;
    std::string uri = conn.request_headers().at("REQUEST_URI");
#include "pages/index_tpl.hpp"

    conn.write_status(200);
    conn.write_string("Content-Type: text/html\r\n\r\n");
    conn.write_string(out.str());

}

