#pragma once

#include <string>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include "server.hpp"

static inline std::unordered_map<std::string, std::string> parse_cookies(const std::string& cookie_str) {
    std::unordered_map<std::string, std::string> cookies;
    std::istringstream stream(cookie_str);
    std::string pair;
    while (std::getline(stream, pair, ';')) {
        auto eq_pos = pair.find('=');
        if (eq_pos != std::string::npos) {
            std::string key = pair.substr(0, eq_pos);
            std::string value = pair.substr(eq_pos + 1);
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            cookies[key] = value;
        }
    }
    return cookies;
}

static bool are_cookies_accepted(Connection & conn)
{
    bool accepted = false;
    const auto& headers = conn.request_headers();
    auto it = headers.find("HTTP_COOKIE");
    if (it != headers.cend()) {
        auto cookies = parse_cookies(it->second);
        auto it2 = cookies.find("cookie_consent");
        if (it2 != cookies.cend()) {
            accepted = it2->second == "full";
        }
    }
    return accepted;
}