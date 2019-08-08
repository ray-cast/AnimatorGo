// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2017 NVIDIA Corporation. All rights reserved.

#include "cudamanager/PxGpuCopyDesc.h"
#include "foundation/PxSimpleTypes.h"

#include <cuda.h>

using namespace physx;

extern "C" __host__ void initUtilKernels() {}

extern "C" __global__
void Saturate( )
{
    // NOP
}

__device__
void performCopy( const physx::PxGpuCopyDesc& desc, uint32_t totalBlocks )
{
	if( desc.type == physx::PxGpuCopyDesc::DeviceMemset32 )
	{
		uint32_t *dest = (uint32_t*) desc.dest;
		uint32_t wordCount = desc.bytes >> 2;
		size_t word = blockIdx.x * blockDim.x + threadIdx.x;
		size_t stride = blockDim.x * totalBlocks;
		for( ; word < wordCount ; word += stride )
			dest[ word ] = desc.source;
		return;
	}

	/* The idea here is to maximize throughput with minimal register and thread counts */
	/* Manually unrolled 4 times, the compiler refuses to do it for me */

	if( (desc.source & 0x7) != 0 || (desc.dest & 0x7) != 0 || (desc.bytes & 0x7) != 0)
	{
		/* Input is word aligned */

		uint32_t *dest = (uint32_t*) desc.dest;
		uint32_t *source = (uint32_t*) desc.source;
		uint32_t wordCount = desc.bytes >> 2;
		size_t word = blockIdx.x * blockDim.x + threadIdx.x;
		size_t stride = blockDim.x * totalBlocks;
		while( word < wordCount )
		{
			uint32_t a0, a1, a2, a3, a4, a5;
			a0 = source[ word ];
			if( word + stride < wordCount )
				a1 = source[ word + stride ];
			if( word + stride*2 < wordCount )
				a2 = source[ word + stride*2 ];
			if( word + stride*3 < wordCount )
				a3 = source[ word + stride*3 ];
			if( word + stride*4 < wordCount )
				a4 = source[ word + stride*4 ];
			if( word + stride*5 < wordCount )
				a5 = source[ word + stride*5 ];

			dest[ word ] = a0;
			if( word + stride < wordCount )
				dest[ word + stride ] = a1;
			if( word + stride*2 < wordCount )
				dest[ word + stride*2 ] = a2;
			if( word + stride*3 < wordCount )
				dest[ word + stride*3 ] = a3;
			if( word + stride*4 < wordCount )
				dest[ word + stride*4 ] = a4;
			if( word + stride*5 < wordCount )
				dest[ word + stride*5 ] = a5;

			word += stride*6;
		}
	}
	else
	{
		/* Input is DWord aligned */

		uint2 *dest = (uint2*) desc.dest;
		uint2 *source = (uint2*) desc.source;
		uint32_t dwordCount = desc.bytes >> 3;
		size_t word = blockIdx.x * blockDim.x + threadIdx.x;
		size_t stride = blockDim.x * totalBlocks;
		while( word < dwordCount )
		{
			uint2 a0, a1, a2, a3, a4, a5;
			a0 = source[ word ];
			if( word + stride < dwordCount )
				a1 = source[ word + stride ];
			if( word + stride*2 < dwordCount )
				a2 = source[ word + stride*2 ];
			if( word + stride*3 < dwordCount )
				a3 = source[ word + stride*3 ];
			if( word + stride*4 < dwordCount )
				a4 = source[ word + stride*4 ];
			if( word + stride*5 < dwordCount )
				a5 = source[ word + stride*5 ];

			dest[ word ] = a0;
			if( word + stride < dwordCount )
				dest[ word + stride ] = a1;
			if( word + stride*2 < dwordCount )
				dest[ word + stride*2 ] = a2;
			if( word + stride*3 < dwordCount )
				dest[ word + stride*3 ] = a3;
			if( word + stride*4 < dwordCount )
				dest[ word + stride*4 ] = a4;
			if( word + stride*5 < dwordCount )
				dest[ word + stride*5 ] = a5;

			word += stride*6;
		}
	}

	__threadfence_system();
}

extern "C" __global__
void MemCopyAsync( physx::PxGpuCopyDesc desc )
{
	performCopy( desc, gridDim.x );
}


extern "C" __global__
void MemCopyBatchedAsync( physx::PxGpuCopyDesc *desc )
{
	__shared__ physx::PxGpuCopyDesc sdesc;

	if( threadIdx.x < sizeof(physx::PxGpuCopyDesc) / sizeof(uint32_t) )
	{
		uint32_t *dest = (uint32_t*)&sdesc;
		uint32_t *source = (uint32_t*)(desc + blockIdx.y);
		dest[ threadIdx.x ] = source[ threadIdx.x ];
		__threadfence_block();
	}
	__syncthreads();

	performCopy( sdesc, gridDim.x );
}
