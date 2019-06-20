#include "logging.h"

#include <array>
#include <ctime>


std::ostream& operator <<(std::ostream& lhs, Timestamp rhs)
{
    auto utc_time = std::time(nullptr);
    auto* gm_time = std::gmtime(&utc_time);
    std::array<char, 1024> time_string;
    std::strftime(time_string.data(),
                  time_string.size(),
                  rhs.format,
                  gm_time);
    return lhs << time_string.data();
}

KeyValueType<std::string> KeyValue(char const* key, char const* value)
{
    KeyValueType<std::string> kv(key, value);
    return KeyValueType<std::string>(key, value);
}

KeyValueStream operator<<(KeyValueStream lhs, KeyValueType<std::string> const& rhs)
{
    *lhs.stream << lhs.delimiter << '"' << rhs.key << '"' << ": " << '"' << rhs.value << '"';
    lhs.delimiter = ", ";
    return lhs;
}
