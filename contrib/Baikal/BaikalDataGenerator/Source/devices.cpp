#include "devices.h"

std::vector<CLWDevice> GetDevices()
{
    std::vector<CLWDevice> devices;

    std::vector<CLWPlatform> platforms;
    CLWPlatform::CreateAllPlatforms(platforms);

    for (const auto& platform : platforms)
    {
        for (auto i = 0u; i < platform.GetDeviceCount(); i++)
        {
            if (platform.GetDevice(i).GetType() == CL_DEVICE_TYPE_GPU)
            {
                devices.push_back(platform.GetDevice(i));
            }
        }
    }

    return devices;
}
