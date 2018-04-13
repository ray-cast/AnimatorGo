#ifndef OCTOON_SERIALIZABLE_H
#define OCTOON_SERIALIZABLE_H

#include <octoon/io/istream.h>
#include <octoon/io/ostream.h>

namespace octoon
{
    namespace io
    {
        class serializable
        {
        public:
            virtual void serialize(istream& in);
            virtual void deserialize(ostream& out);
        };
    }
}


#endif 