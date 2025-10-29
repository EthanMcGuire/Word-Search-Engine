#pragma once

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <vector>

std::string stringToLower(std::string str);
std::string vectorToString(std::vector<unsigned char> vec);
std::string bufferToHex(const unsigned char* data, size_t length);

#endif