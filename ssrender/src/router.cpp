#include "router.hpp"

#include "index.hpp"
#include "roll.hpp"
#include "errors.hpp"

void route(Connection & conn)
{
    std::string uri = conn.request_headers().at("REQUEST_URI");
    if (uri == "/") index(conn);
    else if (uri == "/roll") roll(conn);
    else error_404(conn);
}