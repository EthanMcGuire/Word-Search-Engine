#pragma once

#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include "json/json_fwd.hpp"
#include <string>
#include <fstream>

using json = nlohmann::json;

/// @brief Reads and parses a JSON file.
/// @param path File path.
/// @return The JSON data.
json parseJsonFile(std::string path);

/// @brief Reads and parses a JSON file, returning its top level JSON object.
/// @param path File path.
/// @param jsonField The top-level JSON field to parse.
/// @return The top-level JSON given by jsonField.
json parseJsonFileTop(std::string path, std::string jsonField);

#endif