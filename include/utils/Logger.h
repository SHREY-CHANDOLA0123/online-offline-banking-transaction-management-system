#ifndef LOGGER_H
#define LOGGER_H
#include <string>
#include <vector>
#include <fstream>
#include <ctime>

class Logger
{
    static std::vector<std::string>& store() { static std::vector<std::string> lines; return lines; }
public:
    static std::string timeNow(bool dateToo = true)
    {
        char b[32]; time_t t = time(nullptr);
        strftime(b, sizeof b, dateToo ? "%Y-%m-%d %H:%M:%S" : "%H:%M:%S", localtime(&t));
        return b;
    }
    static void log(const std::string& message)   // kept in memory for the control room and appended to data/server.log
    {
        std::string line = timeNow(false) + "  " + message;
        store().push_back(line); if (store().size() > 80) store().erase(store().begin());
        std::ofstream("data/server.log", std::ios::app) << timeNow() << "  " << message << '\n';
    }
    static const std::vector<std::string>& recent() { return store(); }
};
#endif
