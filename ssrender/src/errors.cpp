#include "errors.hpp"
#include <sstream>
void error_404(Connection & conn)
{
    std::ostringstream out;
    std::string uri = conn.request_headers().at("REQUEST_URI");
#include "pages/404_tpl.hpp"
    conn.write_status(404);
    conn.write_string("Content-Type: text/html\r\n\r\n");
    conn.write_string(out.str());
}