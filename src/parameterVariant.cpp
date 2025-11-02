#include "parameterVariant.hpp"
#include <cmath>
#include <stdexcept>

/// @brief Converts the given parameter to the best possible type it can be converted to.
/// @param paramStr The parameter to convert.
/// @return The Message parameter, containing the final type.
ParameterVariant Parameter::convertToBestType(const std::string& paramStr)
{
    try
    {
        //Attempt to cast to double
        double dValue = std::stod(paramStr);

        //Integer?
        if (std::floor(dValue) == dValue)
        {
            return ParameterVariant(static_cast<int>(dValue));
        }

        //Double
        return ParameterVariant(dValue);
    }
    catch (std::invalid_argument& e) 
    {
        //Return as string
        return ParameterVariant(paramStr);
    }
}

/// @brief Attempts to convert the variant parameter to a double.
/// @param param The parameter to convert.
/// @return The double value.
/// @exception Throws a std::bad_variant_access exception if param is not a number.
double Parameter::convertParameterToDouble(ParameterVariant param)
{
    double x = 0;

    if (std::holds_alternative<double>(param)) 
    {
        x = std::get<double>(param);
    } 
    else if (std::holds_alternative<int>(param)) 
    {
        x = static_cast<double>(std::get<int>(param));
    } 
    else 
    {
        throw std::bad_variant_access();
    }

    return x;
}