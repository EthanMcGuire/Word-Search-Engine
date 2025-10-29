#include "stringUtils.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>

std::string stringToLower(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return std::tolower(c); });

    return str;
}

std::string vectorToString(std::vector<unsigned char> vec)
{
    return std::string(vec.begin(), vec.end()) + "\0";
}

std::string bufferToHex(const unsigned char* data, size_t length)
{
    if (data == nullptr)
    {
        return "";
    }
    
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');

    for (size_t i = 0; i < length; ++i)
    {
        oss << std::setw(2) << static_cast<int>(data[i]);
    }

    return oss.str();
}