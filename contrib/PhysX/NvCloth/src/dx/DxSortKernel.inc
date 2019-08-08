interface ISortElements
{
	int get(int index);
	void set(int index, int value);
	void swap();
};

//Reduce - BlockSize * sizeof(uint4)
//Scan - (2 + BlockSize) * sizeof(uint)
interface ISortShared
{
	uint4 getReduce(int index);
	void setReduce(int index, uint4 value);

	uint getScan(int index);
	void setScan(int index, uint value);
};

#define USE_WARP_LIMIT 1

uint32_t reduceWarps(uint32_t threadIdx, uint32_t laneIdx, uint32_t warpIdx, uint32_t warpLimit, uint32_t threadPos, uint32_t threadEnd, ISortElements sortElements, uint32_t bit, uniform bool bOutput, uint32_t scanOut, ISortShared sortShared)
{
	const uint32_t laneMask = (1u << laneIdx) - 1;
	const uint32_t mask1 = (threadIdx & 1) - 1;
	const uint32_t mask2 = !!(threadIdx & 2) - 1;
	const uint32_t mask4 = !!(threadIdx & 4) - 1;
	const uint32_t mask8 = !!(threadIdx & 8) - 1;

	uint32_t key = threadPos < threadEnd ? sortElements.get(threadPos) : 0xFFFFFFFF;
	uint32_t keyDigit = (key >> bit) & 0x0F;

#if USE_WARP_LIMIT
	if (warpIdx < warpLimit)
#endif
		sortShared.setReduce(threadIdx, uint4(!!(keyDigit & 1) << laneIdx, !!(keyDigit & 2) << laneIdx, !!(keyDigit & 4) << laneIdx, !!(keyDigit & 8) << laneIdx));
	GroupMemoryBarrierWithGroupSync();
#if USE_WARP_LIMIT
	if (warpIdx < warpLimit)
#endif
		if (laneIdx < 16) sortShared.setReduce(threadIdx, sortShared.getReduce(threadIdx) | sortShared.getReduce(threadIdx + 16));
	GroupMemoryBarrierWithGroupSync();
#if USE_WARP_LIMIT
	if (warpIdx < warpLimit)
#endif
		if (laneIdx < 8) sortShared.setReduce(threadIdx, sortShared.getReduce(threadIdx) | sortShared.getReduce(threadIdx + 8));
	GroupMemoryBarrierWithGroupSync();
#if USE_WARP_LIMIT
	if (warpIdx < warpLimit)
#endif
		if (laneIdx < 4) sortShared.setReduce(threadIdx, sortShared.getReduce(threadIdx) | sortShared.getReduce(threadIdx + 4));
	GroupMemoryBarrierWithGroupSync();
#if USE_WARP_LIMIT
	if (warpIdx < warpLimit)
#endif
		if (laneIdx < 2) sortShared.setReduce(threadIdx, sortShared.getReduce(threadIdx) | sortShared.getReduce(threadIdx + 2));
	GroupMemoryBarrierWithGroupSync();
#if USE_WARP_LIMIT
	if (warpIdx < warpLimit)
#endif
		if (laneIdx < 1) sortShared.setReduce(threadIdx, sortShared.getReduce(threadIdx) | sortShared.getReduce(threadIdx + 1));
	GroupMemoryBarrierWithGroupSync();

	uint4 ballot = 0;
#if USE_WARP_LIMIT
	if (warpIdx < warpLimit)
#endif
		ballot = sortShared.getReduce(threadIdx & ~31);

	uint32_t result = 0;
	if (bOutput)
	{
		uint32_t index = 0;
#if USE_WARP_LIMIT
		if (warpIdx < warpLimit)
#endif
		{
			uint32_t bits = ((keyDigit & 1) - 1 ^ ballot[0]) & (!!(keyDigit & 2) - 1 ^ ballot[1]) & (!!(keyDigit & 4) - 1 ^ ballot[2]) & (!!(keyDigit & 8) - 1 ^ ballot[3]);
			index = sortShared.getScan(scanOut + warpIdx + keyDigit * WarpsPerBlock) + countbits(bits & laneMask);
		}
		if (threadPos < threadEnd)
		{
			sortElements.set(index, key);
		}

		GroupMemoryBarrierWithGroupSync(); //to remove gScan read/write race condition
#if USE_WARP_LIMIT
		if (warpIdx < warpLimit)
#endif
			if (laneIdx < 16)
			{
				int scanIdx = scanOut + warpIdx + laneIdx * WarpsPerBlock;
				sortShared.setScan(scanIdx, sortShared.getScan(scanIdx) + countbits((mask1 ^ ballot[0]) & (mask2 ^ ballot[1]) & (mask4 ^ ballot[2]) & (mask8 ^ ballot[3])));
			}
	}
	else
	{
//#if USE_WARP_LIMIT
		if (warpIdx < warpLimit)
//#endif
			result = countbits((mask1 ^ ballot[0]) & (mask2 ^ ballot[1]) & (mask4 ^ ballot[2]) & (mask8 ^ ballot[3]));
	}
	return result;
}

//TODO: check & fix shared memory bank conflicts if needed!
void radixSort_BitCount(uint32_t threadIdx, uint32_t n, ISortElements sortElements, uint32_t startBit, uint32_t endBit, ISortShared sortShared)
{
	const uint32_t warpIdx = threadIdx >> 5;
	const uint32_t laneIdx = threadIdx & 31;

	const uint32_t WarpsTotal = ((n + 31) >> 5);
	const uint32_t WarpsRemain = WarpsTotal % WarpsPerBlock;
	const uint32_t WarpsFactor = WarpsTotal / WarpsPerBlock;
	const uint32_t WarpsSelect = (warpIdx < WarpsRemain);
	const uint32_t WarpsCount = WarpsFactor + WarpsSelect;
	const uint32_t WarpsOffset = warpIdx * WarpsCount + WarpsRemain * (1 - WarpsSelect);
	const uint32_t warpBeg = (WarpsOffset << 5);
	const uint32_t warpEnd = min(warpBeg + (WarpsCount << 5), n);
	const uint32_t threadBeg = warpBeg + laneIdx;

	const uint32_t ScanCount = WarpsPerBlock * 16;

	for (uint32_t bit = startBit; bit < endBit; bit += 4) // radix passes (4 bits each)
	{
		// gather bucket histograms per warp
		uint32_t warpCount = 0;
		uint32_t i;
		uint32_t threadPos;
		for (i = 0, threadPos = threadBeg; i < WarpsFactor; ++i, threadPos += 32)
		{
			warpCount += reduceWarps(threadIdx, laneIdx, warpIdx, WarpsPerBlock, threadPos, warpEnd, sortElements, bit, false, 0, sortShared);
		}
		if (WarpsRemain > 0)
		{
			warpCount += reduceWarps(threadIdx, laneIdx, warpIdx, WarpsRemain, threadPos, warpEnd, sortElements, bit, false, 0, sortShared);
		}

		if (laneIdx < 16)
		{
			sortShared.setScan(1 + warpIdx + laneIdx * WarpsPerBlock, warpCount);
		}
		GroupMemoryBarrierWithGroupSync();

		// prefix sum of histogram buckets
		if (threadIdx == 0)
		{
			sortShared.setScan(0, 0);
			sortShared.setScan(ScanCount + 1, 0);
		}

		uint32_t scanIn = 1;
		uint32_t scanOut = 2 + ScanCount;

		//if (threadIdx < ScanCount)
		//	bSortElements[n * 2 + threadIdx] = gScan[scanIn + threadIdx];

		{
			[unroll]
			for (uint32_t offset = 1; offset < ScanCount; offset *= 2)
			{
				if (threadIdx < ScanCount)
				{
					if (threadIdx >= offset)
						sortShared.setScan(scanOut + threadIdx, sortShared.getScan(scanIn + threadIdx) + sortShared.getScan(scanIn + threadIdx - offset));
					else
						sortShared.setScan(scanOut + threadIdx, sortShared.getScan(scanIn + threadIdx));
				}
				// swap double buffer indices  
				uint32_t temp = scanOut;
				scanOut = scanIn;
				scanIn = temp;
				GroupMemoryBarrierWithGroupSync();
			}
		}

		//if (threadIdx < ScanCount)
		//	bSortElements[n * 2 + ScanCount + threadIdx] = gScan[scanIn + threadIdx];

		scanIn -= 1; //make scan exclusive!
		// split indices
		for (i = 0, threadPos = threadBeg; i < WarpsFactor; ++i, threadPos += 32)
		{
			reduceWarps(threadIdx, laneIdx, warpIdx, WarpsPerBlock, threadPos, warpEnd, sortElements, bit, true, scanIn, sortShared);
		}
		if (WarpsRemain > 0)
		{
			reduceWarps(threadIdx, laneIdx, warpIdx, WarpsRemain, threadPos, warpEnd, sortElements, bit, true, scanIn, sortShared);
		}

		//GroupMemoryBarrierWithGroupSync();
		sortElements.swap();
	}
}
