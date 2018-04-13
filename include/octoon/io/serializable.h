#ifndef OCTOON_SERIALIZABLE_H
#define OCTOON_SERIALIZABLE_H

#include <octoon/io/istream.h>
#include <octoon/io/binary_reader.h>
#include <octoon/io/ostream.h>
#include <octoon/io/binary_writer.h>

namespace octoon
{
    namespace io
    {
        class serializable
        {
        public:
            virtual void serialize(BinaryWriter& out) = 0;
        };
    }
}


#endif 