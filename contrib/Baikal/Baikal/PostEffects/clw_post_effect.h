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

#include "post_effect.h"

#include "CLW.h"
#include "Output/clwoutput.h"
#include "Utils/clw_class.h"

#include <string>
#include <unordered_map>

namespace Baikal
{
    class CLProgramManager;
    /**
    \brief Post effects partial implementation based on CLW framework.
    */
    class ClwPostEffect : public PostEffect, protected ClwClass
    {
    public:
#ifdef BAIKAL_EMBED_KERNELS
        // Constructor, receives CLW context
        ClwPostEffect(CLWContext context, const CLProgramManager *program_manager, std::string const& name,
            std::string const& source, std::unordered_map<char const*, char const*> const& headers);
#else
        ClwPostEffect(CLWContext context, const CLProgramManager *program_manager, std::string const& file_name);
#endif
    };

#ifdef BAIKAL_EMBED_KERNELS
    inline ClwPostEffect::ClwPostEffect(CLWContext context, const CLProgramManager *program_manager, std::string const& name,
        std::string const& source, std::unordered_map<char const*, char const*> const& headers)
        : ClwClass(context, program_manager, name, source, headers)
    {
    }
#else
    inline ClwPostEffect::ClwPostEffect(CLWContext context, const CLProgramManager *program_manager, std::string const& file_name)
        : ClwClass(context, program_manager, file_name)
    {
    }
#endif
}
