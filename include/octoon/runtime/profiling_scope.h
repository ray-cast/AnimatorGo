#ifndef OCTOON_PROFILING_SCOPE_H_
#define OCTOON_PROFILING_SCOPE_H_

#include <chrono>
#include <iostream>

namespace octoon
{
    class ProfilingScope final
    {
    public:
        ProfilingScope()
        {
            start = std::chrono::high_resolution_clock::now();
        }

        ~ProfilingScope()
        {
            stop();
        }

        void stop()
        {
            auto end = std::chrono::high_resolution_clock::now();

            auto _start = std::chrono::time_point_cast<std::chrono::microseconds>(start).time_since_epoch().count();
            auto _end = std::chrono::time_point_cast<std::chrono::microseconds>(end).time_since_epoch().count();

            std::cout << "duration:" << _end - _start << "us" << std::endl;
        }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> start;
    };
}

#endif