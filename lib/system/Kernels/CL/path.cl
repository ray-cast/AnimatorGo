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
#ifndef PATH_CL
#define PATH_CL

#include <../../system/Kernels/CL/common.cl>
#include <../../system/Kernels/CL/payload.cl>
#include <../../system/Kernels/CL/bxdf_flags.cl>

typedef struct _Path
{
    float3 throughput;
    int volume;
    int flags;
    int active;
    int extra1;
} Path;

typedef enum _PathFlags
{
    kNone = 0x0,
    kKilled = 0x1,
    kScattered = 0x2,
    kOpaque = 0x4
} PathFlags;

INLINE bool Path_IsScattered(__global Path const* path)
{
    return path->flags & kScattered;
}

INLINE bool Path_IsAlive(__global Path const* path)
{
    return ((path->flags & kKilled) == 0);
}

INLINE bool Path_ContainsOpacity(__global Path const* path)
{
    return path->flags & kOpaque;
}

INLINE void Path_ClearScatterFlag(__global Path* path)
{
    path->flags &= ~kScattered;
}

INLINE void Path_SetScatterFlag(__global Path* path)
{
    path->flags |= kScattered;
}

INLINE void Path_SetOpacityFlag(__global Path* path)
{
    path->flags |= kOpaque;
}

INLINE void Path_ClearBxdfFlags(__global Path* path)
{
    path->flags &= (kKilled | kScattered | kOpaque);
}

INLINE int Path_GetBxdfFlags(__global Path const* path)
{
    return path->flags >> 8;
}

INLINE int Path_SetBxdfFlags(__global Path* path, int flags)
{
    return path->flags |= (flags << 8);
}

INLINE void Path_Restart(__global Path* path)
{
    path->flags = 0;
}

INLINE int Path_GetVolumeIdx(__global Path const* path)
{
    return path->volume;
}

INLINE void Path_SetVolumeIdx(__global Path* path, int volume_idx)
{
    path->volume = volume_idx;
}

INLINE float3 Path_GetThroughput(__global Path const* path)
{
    float3 t = path->throughput;
    return t;
}

INLINE void Path_MulThroughput(__global Path* path, float3 mul)
{
    path->throughput *= mul;
}

INLINE void Path_Kill(__global Path* path)
{
    path->flags |= kKilled;
}

INLINE void Path_AddContribution(__global Path* path, __global float3* output, int idx, float3 val)
{
    output[idx] += Path_GetThroughput(path) * val;
}

INLINE bool Path_IsSpecular(__global Path const* path)
{
    int flags = Path_GetBxdfFlags(path);
    return (flags & kBxdfFlagsSingular) == kBxdfFlagsSingular;
}

INLINE void Path_SetFlags(DifferentialGeometry* diffgeo, GLOBAL Path* restrict path)
{
    Path_ClearBxdfFlags(path);
    Path_SetBxdfFlags(path, Bxdf_GetFlags(diffgeo));
}

#endif
