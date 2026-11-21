#ifndef INPUT_VALIDATION_H
#define INPUT_VALIDATION_H
#include <string>
#include <algorithm>
#include <vector>
#include <sstream>
#include <cctype>

class InputValidation
{
public:
    // Keep only safe characters (letters, digits, - _ : space) from anything a client sends
    static std::string sanitize(const std::string& s)
    {
        std::string o;
        for (char c : s) if (isalnum((unsigned char)c) || c == '-' || c == '_' || c == ':' || c == ' ') o += c;
        return o;
    }
    static bool isValidAmount(long long paise) { return paise > 0 && paise <= 100000000; }   // up to Rs 10,00,000
    static bool isValidPin(const std::string& p) { return p.size() == 4 && std::all_of(p.begin(), p.end(), ::isdigit); }
    static std::vector<std::string> split(const std::string& s, char d)
    {
        std::vector<std::string> v; std::string t; std::stringstream ss(s);
        while (std::getline(ss, t, d)) v.push_back(t);
        return v;
    }
};
#endif
