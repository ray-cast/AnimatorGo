/**********************************************************************
Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
********************************************************************/
#pragma once

#include <iostream>

namespace Baikal
{
    template <typename Stream>
    inline
        void Log(Stream&)
    {
    }

    template<typename Stream, typename F, typename ...U>
    inline
    void Log(Stream& stream, F&& f, U&&... u)
    {
#ifdef ENABLE_LOG
        stream << std::forward<F>(f);
        Log(stream, std::forward<U>(u)...);
#endif
    }

    template<typename F, typename ...U>
    inline
    void LogInfo(F&& f, U&&... u)
    {
        Log(std::cout, std::forward<F>(f), std::forward<U>(u)...);
    }

    template<typename F, typename ...U>
    inline
    void LogError(F&& f, U&&... u)
    {
        Log(std::cerr, std::forward<F>(f), std::forward<U>(u)...);
    }
}