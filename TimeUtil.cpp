#include "TimeUtil.h"

#include <ctime>
#include <iomanip>
#include <sstream>

std::string getCurrentTimestamp()
{
    std::time_t now = std::time(nullptr);
    std::tm tmValue{};
#if defined(_WIN32)
    localtime_s(&tmValue, &now);
#else
    localtime_r(&now, &tmValue);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tmValue, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string getCurrentTimestampDashed()
{
    std::time_t now = std::time(nullptr);
    std::tm tmValue{};
#if defined(_WIN32)
    localtime_s(&tmValue, &now);
#else
    localtime_r(&now, &tmValue);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tmValue, "%Y-%m-%d-%H-%M-%S");
    return oss.str();
}
