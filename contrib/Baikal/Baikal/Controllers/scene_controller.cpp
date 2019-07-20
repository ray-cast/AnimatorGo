#include <cstdint>

namespace Baikal
{
    static std::uint32_t g_next_id = 0;

    std::uint32_t GetNextControllerId()
    {
        return g_next_id++;
    }

    void ResetControllerId()
    {
        g_next_id = 0;
    }

}
