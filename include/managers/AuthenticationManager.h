#ifndef AUTHENTICATION_MANAGER_H
#define AUTHENTICATION_MANAGER_H
#include <string>
#include <fstream>
#include <unordered_map>
#include "../utils/InputValidation.h"

// PIN login. PINs live in data/users.dat (plain text: fine for a classroom simulation, never for real money).
class AuthenticationManager
{
    std::unordered_map<std::string, std::string> pins;
public:
    void load()
    {
        std::ifstream f("data/users.dat"); std::string line;
        while (std::getline(f, line)) { auto v = InputValidation::split(line, '|'); if (v.size() == 2) pins[v[0]] = v[1]; }
    }
    void save() const { std::ofstream f("data/users.dat"); for (const auto& p : pins) f << p.first << '|' << p.second << '\n'; }
    void ensureUser(const std::string& accountId, const std::string& pin = "1234")
    { if (!pins.count(accountId)) { pins[accountId] = pin; save(); } }
    bool login(const std::string& accountId, const std::string& pin) const
    { auto it = pins.find(accountId); return it != pins.end() && it->second == pin; }
};
#endif
