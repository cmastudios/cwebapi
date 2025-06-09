#include "index.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include "cookies.hpp"

void index(Connection & conn)
{
    std::ostringstream out;
    std::string uri = conn.request_headers().at("REQUEST_URI");
    bool set_cookie = false;
    bool cookies_accepted = are_cookies_accepted(conn);
    if (conn.request_headers().at("REQUEST_METHOD") == "POST")
    {
        auto body = conn.read_string(std::stoi(conn.request_headers().at("CONTENT_LENGTH")));
        if (body == "cookie_consent=full") {
            set_cookie = true;
            cookies_accepted = true;
        }
    }

#include "pages/index_tpl.hpp"

    conn.write_status(200);
    if (set_cookie) {
        time_t expiresS;
        (void)time(&expiresS);
        expiresS += 365 * 86400;
        struct tm * expiresTm = gmtime(&expiresS);
        char expiresStr[100];
        strftime(expiresStr, 100, "%a, %Y %m %d %H:%M:%S GMT", expiresTm);
        conn.write_string("Set-Cookie: cookie_consent=full; Expires=" + std::string(expiresStr) + "\r\n");
    }
    conn.write_string("Content-Type: text/html\r\n\r\n");
    conn.write_string(out.str());

}

