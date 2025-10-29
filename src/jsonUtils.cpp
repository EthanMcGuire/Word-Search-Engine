#include "jsonUtils.hpp"
#include "json/json.hpp"
#include <fstream>

using json = nlohmann::json;

/// @brief Reads and parses a JSON file.
/// @param path File path.
/// @return The JSON data.
json parseJsonFile(std::string path)
{
    std::ifstream fin;
    json data;

    fin.open(path, std::ifstream::in);
    data = json::parse(fin);
    fin.close();

    return data;
}

/// @brief Reads and parses a JSON file, returning its top level JSON object.
/// @param path File path.
/// @param jsonField The top-level JSON field to parse.
/// @return The top-level JSON given by jsonField.
json parseJsonFileTop(std::string path, std::string jsonField)
{
    std::ifstream fin;
    json data;

    fin.open(path, std::ifstream::in);
    data = json::parse(fin);
    fin.close();

    return data[jsonField];
}