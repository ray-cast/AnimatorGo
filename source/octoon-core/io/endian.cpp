#include <octoon/io/endian.h>

namespace octoon
{
    namespace io
    {
        std::once_flag Endian::oc;
		EndianType Endian::type;
    }
}
