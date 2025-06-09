#include "server.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <unistd.h>
#include <err.h>
#include <signal.h>
#include <unordered_map>
#include <string>
#include <iostream>
#include <array>
#include <vector>
#include "router.hpp"

class ScgiConnection : public Connection
{
public:
    ScgiConnection(int fd) : m_fd(fd) {
        m_buffer.reserve(1024);
    }

    int content_length() override
    {
        // For simplicity, we assume the content length is always 0 in this example.
        return 0;
    }

    int read(char * buffer, int size) override
    {
        int remaining = size;
        bool read_ok = true;
        while (remaining > 0 && read_ok)
        {
            if (m_buffer.empty())
            {
                read_ok = read_into_buffer();
            }
            int read_from_buffer = std::min(remaining, static_cast<int>(m_buffer.size()));
            std::copy(m_buffer.begin(), m_buffer.begin() + read_from_buffer, buffer);
            m_buffer.erase(m_buffer.begin(), m_buffer.begin() + read_from_buffer);
            remaining -= read_from_buffer;
        }
        return size - remaining;
    }

    int write_status(int status) override
    {
        char response[128];
        int len = snprintf(response, sizeof(response), "Status: %d\r\n", status);
        return write(response, len);
    }

    int write(const char * buffer, int size) override
    {
        int res = ::send(m_fd, buffer, size, 0);
        if (res < 0) throw std::runtime_error(strerror(errno));
        return res;
    }

    bool read_headers()
    {
        size_t len = 0U;
        char c = '0';
        while (read(&c, 1) == 1 && c != ':' && isalnum(c))
        {
            len = len * 10U + (c - '0');
        }
        bool key_mode = true;
        std::string key;
        std::string value;
        for (size_t i = 0; i < len; ++i)
        {
            if (read(&c, 1) != 1) return false;
            if (c == '\0')
            {
                if (!key_mode)
                {
                    m_headers[key] = value;
                    key.clear();
                    value.clear();
                }
                key_mode = !key_mode;
            }
            else
            {
                if (key_mode)
                {
                    key += c;
                }
                else
                {
                    value += c;
                }
            }
        }

        return (read(&c, 1) == 1) && (c == ',');
    }
    const std::unordered_map<std::string, std::string> & request_headers() override { return m_headers; }
private:
    int m_fd;
    std::vector<char> m_buffer;
    std::unordered_map<std::string, std::string> m_headers;

    bool read_into_buffer()
    {
        char temp[1024];
        int bytes_read = ::recv(m_fd, temp, sizeof(temp), 0);
        if (bytes_read < 0) throw std::runtime_error(strerror(errno));
        if (bytes_read > 0)
        {
            m_buffer.insert(m_buffer.end(), temp, temp + bytes_read);
        }
        return bytes_read >= 1024;
    }
};

void run_scgi_server(void)
{
    signal(SIGPIPE, SIG_IGN);

    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock == -1) err(1, "socket");

    struct sockaddr_un sa;
    memset(&sa, 0, sizeof(sa));
    sa.sun_family = AF_UNIX;
    strncpy(sa.sun_path, "/tmp/scgi.sock", sizeof(sa.sun_path) - 1);
    unlink(sa.sun_path); // Remove the socket file if it already exists

    if (bind(sock, (struct sockaddr *)&sa, sizeof(sa)) == -1) err(1, "bind");

    chmod(sa.sun_path, 0777);

    if (listen(sock, 4U) == -1) err(1, "listen");

    while (true)
    {
        struct sockaddr_in remote_addr;
        socklen_t remote_addr_size = sizeof(remote_addr);
        int conn = accept(sock, (struct sockaddr *)&remote_addr, &remote_addr_size);

        if (conn == -1) err(1, "accept");

        try
        {
            ScgiConnection connection(conn);
            if (connection.read_headers())
            {
                route(connection);
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
        // std::cout << connection.read_headers() << std::endl;
        // std::cout << "Received headers:" << std::endl;
        // for (const auto &header : connection.m_headers)
        // {
        //     std::cout << header.first << ": " << header.second << std::endl;
        // }

        if (close(conn) == -1) err(1, "close");
    }

    unlink(sa.sun_path);
}