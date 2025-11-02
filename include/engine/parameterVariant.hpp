#pragma once

#ifndef PARAMETER_VARIANT_H
#define PARAMETER_VARIANT_H

#include <variant>
#include <string>

using ParameterVariant = std::variant<int, double, std::string>;

namespace Parameter
{
    /// @brief Converts the given parameter to the best possible type it can be converted to.
    /// @param paramStr The parameter to convert.
    /// @return The Message parameter, containing the final type.
    ParameterVariant convertToBestType(const std::string& paramStr);

    /// @brief Attempts to convert the variant parameter to a double.
    /// @param param The parameter to convert.
    /// @return The double value.
    /// @exception Throws a std::bad_variant_access exception if param is not a number.
    double convertParameterToDouble(ParameterVariant param);
}

#endif