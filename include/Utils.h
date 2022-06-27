#pragma once

#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>

static bool to_bool(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    std::istringstream is{str};
    bool b;
    is >> std::boolalpha >> b;
    return b;
}

static bool to_bool(const char* c_str)
{
    std::string str{c_str};
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    std::istringstream is{str};
    bool b;
    is >> std::boolalpha >> b;
    return b;
}

inline static bool isEpsPositive(double x, double eps = 1e-05)
{
    return x > eps;
}