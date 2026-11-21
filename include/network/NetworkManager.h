#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H
#include <string>
#include <sstream>
#ifdef _WIN32
#include <winsock2.h>
typedef SOCKET sock_t;
#define CLOSESOCK closesocket
#define BAD_SOCK INVALID_SOCKET
#else
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <unistd.h>
#include <csignal>
typedef int sock_t;
#define CLOSESOCK close
#define BAD_SOCK (-1)
#endif
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

struct HttpRequest { std::string method, path, body; };

// A tiny single-threaded HTTP server on plain sockets (this is the project's "socket-based networking").
class NetworkManager
{
    sock_t server = BAD_SOCK, client = BAD_SOCK;
public:
    bool start(int port)
    {
#ifdef _WIN32
        WSADATA w; WSAStartup(MAKEWORD(2, 2), &w);
#else
        signal(SIGPIPE, SIG_IGN);
#endif
        server = socket(AF_INET, SOCK_STREAM, 0); int one = 1;
        setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (const char*)&one, sizeof one);
        sockaddr_in a{}; a.sin_family = AF_INET; a.sin_addr.s_addr = htonl(INADDR_ANY); a.sin_port = htons(port);
        return bind(server, (sockaddr*)&a, sizeof a) >= 0 && listen(server, 16) >= 0;
    }
    bool nextRequest(HttpRequest& r, int waitMs = -1)   // waits for one request (waitMs < 0 = forever); the caller must answer it with respond()
    {
        if (waitMs >= 0) {
            fd_set fds; FD_ZERO(&fds); FD_SET(server, &fds); timeval tv{waitMs / 1000, (waitMs % 1000) * 1000};
            if (select((int)server + 1, &fds, nullptr, nullptr, &tv) <= 0) return false;
        }
        client = accept(server, nullptr, nullptr); if (client == BAD_SOCK) return false;
#ifdef _WIN32
        DWORD to = 1500; setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (const char*)&to, sizeof to);
#else
        timeval to{2, 0}; setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof to);
#endif
        std::string req; char buf[4096]; size_t hend = std::string::npos, need = 0;
        while (true) {
            int n = recv(client, buf, sizeof buf, 0); if (n <= 0) break; req.append(buf, n);
            if (hend == std::string::npos && (hend = req.find("\r\n\r\n")) != std::string::npos) {
                hend += 4; std::string h = req.substr(0, hend); for (auto& c : h) c = (char)tolower((unsigned char)c);
                size_t p = h.find("content-length:"); if (p != std::string::npos) { try { need = std::stoul(h.substr(p + 15)); } catch (...) {} }
            }
            if (hend != std::string::npos && req.size() >= hend + need) break;
        }
        if (hend == std::string::npos) { CLOSESOCK(client); return false; }
        std::istringstream line(req); line >> r.method >> r.path; r.body = req.substr(hend);
        return true;
    }
    // Send one HTTP request to another local site (this is how the customer site talks to the bank site).
    static bool call(int port, const std::string& method, const std::string& path, const std::string& body, std::string& out, int timeoutMs = 1500)
    {
        sock_t s = socket(AF_INET, SOCK_STREAM, 0); if (s == BAD_SOCK) return false;
#ifdef _WIN32
        DWORD to = timeoutMs; setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&to, sizeof to); setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (const char*)&to, sizeof to);
#else
        timeval to{timeoutMs / 1000, (timeoutMs % 1000) * 1000}; setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof to); setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &to, sizeof to);
#endif
        sockaddr_in a{}; a.sin_family = AF_INET; a.sin_port = htons(port); a.sin_addr.s_addr = htonl(0x7F000001);
        if (connect(s, (sockaddr*)&a, sizeof a) < 0) { CLOSESOCK(s); return false; }
        std::string req = method + " " + path + " HTTP/1.1\r\nHost: localhost\r\nContent-Length: " + std::to_string(body.size()) + "\r\nConnection: close\r\n\r\n" + body;
        send(s, req.data(), (int)req.size(), MSG_NOSIGNAL);
        std::string resp; char buf[4096]; int n; while ((n = recv(s, buf, sizeof buf, 0)) > 0) resp.append(buf, n);
        CLOSESOCK(s);
        size_t h = resp.find("\r\n\r\n"); if (h == std::string::npos) return false;
        out = resp.substr(h + 4); return resp.rfind("HTTP/1.1 200", 0) == 0;
    }
    void respond(int code, const std::string& type, const std::string& body)
    {
        std::string h = "HTTP/1.1 " + std::to_string(code) + (code == 200 ? " OK" : " Not Found") + "\r\nContent-Type: " + type + "\r\nContent-Length: " + std::to_string(body.size()) + "\r\nCache-Control: no-store\r\nConnection: close\r\n\r\n" + body;
        size_t sent = 0; while (sent < h.size()) { int n = send(client, h.data() + sent, (int)(h.size() - sent), MSG_NOSIGNAL); if (n <= 0) break; sent += n; }
        CLOSESOCK(client);
    }
};
#endif
