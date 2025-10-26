#include "dateUtils.hpp"
#include <ctime>

std::string Date::getDateString()
{
    time_t rawTime;
    struct tm *timeInfo;
    char buffer[80];

    time (&rawTime);
    timeInfo = localtime(&rawTime);

    strftime(buffer, sizeof(buffer), "%m-%d-%y", timeInfo);

    return std::string(buffer);
}