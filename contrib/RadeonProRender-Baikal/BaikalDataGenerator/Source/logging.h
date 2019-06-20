#pragma once

#include <iostream>
#include <string>


/**
 * I/O maninpulator. Inserts a formatted daytime into an std::ostream.
 * Replacement for std::put_time which is not yet supported by all compilers.
 */
struct Timestamp
{
    Timestamp(char const* format = "%Y-%m-%d %H:%M:%S")
        : format(format)
    { }

    char const* format;
};

std::ostream& operator <<(std::ostream& lhs, Timestamp rhs);

// CEE is a JSON/XML structured log format. Search web for "cee format" for details
#define DG_LOG(INSERTERS) \
    std::cout << "[" << Timestamp() << "] @cee: {" << INSERTERS << "}" << std::endl;


/**
 * I/O manipulator. Inserts a key string, expects a value as next string.
 * All inserted values are formatted using JSON notation.
 */
template<class Value>
struct KeyValueType
{
    KeyValueType(char const* key, Value value) : key(key), value(std::move(value)) { }
    char const* key;
    Value value;
};

template<class Value>
KeyValueType<Value> KeyValue(char const* key, Value const& value)
{
    return KeyValueType<Value>(key, value);
}

KeyValueType<std::string> KeyValue(char const* key, char const* value);

struct KeyValueStream
{
    explicit KeyValueStream(std::ostream& stream) : stream(&stream) { }
    std::ostream* stream;
    char const* delimiter = "";
};

template<class Value>
KeyValueStream operator<<(std::ostream& lhs, KeyValueType<Value> const& rhs)
{
    KeyValueStream stream(lhs);
    return stream << rhs;
}

template<class Value>
KeyValueStream operator<<(KeyValueStream lhs, KeyValueType<Value> const& rhs)
{
    *lhs.stream << lhs.delimiter << '"' << rhs.key << '"' << ": " << rhs.value;
    lhs.delimiter = ", ";
    return lhs;
}

KeyValueStream operator<<(KeyValueStream lhs, KeyValueType<std::string> const& rhs);

template<class T>
std::ostream& operator<<(KeyValueStream lhs, T const& rhs)
{
    *lhs.stream << rhs;
    return *lhs.stream;
}
