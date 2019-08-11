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
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.

#include "TripletScheduler.h"
#include <algorithm>
#include "../../src/ps/PsUtilities.h"

using namespace physx;
using namespace nv;

cloth::TripletScheduler::TripletScheduler(Range<const uint32_t[4]> triplets)
: mTriplets(reinterpret_cast<const Vec4u*>(triplets.begin()), reinterpret_cast<const Vec4u*>(triplets.end()))
{
}

// helper functions for TripletScheduler::simd()
namespace
{
	//linear search should be fine for small particlesInBatchSize values
	bool isIndexInBatch(uint32_t* particlesInBatch, int particlesInBatchSize, uint32_t index1, uint32_t index2, uint32_t index3)
	{
		for(int i = 0; i < particlesInBatchSize; i++)
		{
			if(particlesInBatch[i] == 0xffffffff)
				return false;
			if(index1 == particlesInBatch[i] || index2 == particlesInBatch[i] || index3 == particlesInBatch[i])
				return true;
		}
		return false;
	}

	void markIndicesInBatch(uint32_t* particlesInBatch, int particlesInBatchSize, uint32_t index1, uint32_t index2, uint32_t index3)
	{
		for(int i = 0; i < particlesInBatchSize - 2; i++)
		{
			if(particlesInBatch[i] == 0xffffffff)
			{
				NV_CLOTH_ASSERT(i + 2 < particlesInBatchSize);
				particlesInBatch[i] = index1;
				particlesInBatch[i + 1] = index2;
				particlesInBatch[i + 2] = index3;
				break;
			}
		}
	}
}

/*
Group triplets in batches of simdWith so that they can be processed in parallel without referencing the same particle.
Not suitable for simdWidth with large values due to linear search.
Note that sets can be larger that simdWidth, and that particles may be referenced from the same set multiple times, 
	as long as they are not referenced more than once within a batch of simdWidth. The batch resets at the start of 
	each set.
*/
void cloth::TripletScheduler::simd(uint32_t numParticles, uint32_t simdWidth)
{
	PX_UNUSED(numParticles);
	if(mTriplets.empty())
		return;

	const int particlesInBatchSize = simdWidth * 3;
	uint32_t* particlesInBatch = new uint32_t[particlesInBatchSize]; //used to mark used indices in current batch
	int setSize = 0; //number of triplets in current set
	int amountOfPaddingNeeded = 0;

	for(TripletIter tIt = mTriplets.begin(), tEnd = mTriplets.end(); tIt != tEnd; tIt++)
	{
		if(setSize%simdWidth == 0)
		{
			//reset particlesInBatch if we filled the simd width
			memset(particlesInBatch, 0xff, sizeof(uint32_t)*particlesInBatchSize);
		}

		TripletIter tSwap = tIt;

		//Look for the next triplet that doesn't share indices with current batch
		while(isIndexInBatch(particlesInBatch, particlesInBatchSize, tSwap->x, tSwap->y, tSwap->z))
		{
			tSwap++;
			if(tSwap == tEnd)
				break;
		}

		if(tSwap == tEnd)
		{
			//all remaining triplets are share indices with the current batch

			//This doesn't make sense, as it will just introduce an extra set with setSize<simdWidth
			//Should double check though
			//  // trim set to multiple of simdWidth with the hope that the trimmed triples
			//  //  will fit in the next set with a multiple of simdWidth size
			//  if(setSize > (int)simdWidth)
			//  {
			//  	uint32_t overflow = setSize % simdWidth;
			//  	setSize -= overflow;
			//  	tIt -= overflow;
			//  }

			//finish set
			mSetSizes.pushBack(setSize);
			amountOfPaddingNeeded += (static_cast<int>(simdWidth) - (setSize % static_cast<int>(simdWidth))) % static_cast<int>(simdWidth); //last modulo avoids adding padding when setSize%simdWidth==0
			setSize = 0;
			tIt--; //start next set with current triplet
		}
		else
		{
			//add triplet to set
			markIndicesInBatch(particlesInBatch, particlesInBatchSize, tSwap->x, tSwap->y, tSwap->z);
			std::iter_swap(tIt, tSwap); //Place triplet at the end of the current set, so that they in sequence in mTriplets
			setSize++;
		}
	}
	if(setSize)
	{
		//finish last set, if we have one
		mSetSizes.pushBack(setSize);
		amountOfPaddingNeeded += (static_cast<int>(simdWidth) - (setSize % static_cast<int>(simdWidth)))% static_cast<int>(simdWidth);
	}

	// Padding code used to live in SwCloth::setVirtualParticles, now we do it here directly 
	mPaddedTriplets.reserve(mTriplets.size() + amountOfPaddingNeeded);

	{
		Vec4us paddingDummy(static_cast<uint16_t>(numParticles), static_cast<uint16_t>(numParticles) + 1, static_cast<uint16_t>(numParticles) + 2, 0);

		TripletIter tIt = mTriplets.begin();
		//TripletIter tEnd = mTriplets.end();
		SetIter setIt = mSetSizes.begin();
		SetIter setEnd = mSetSizes.end();
		for(; setIt < setEnd; ++setIt)
		{
			for(unsigned int i = 0; i < *setIt; i++)
			{
				mPaddedTriplets.pushBack(*tIt);
				++tIt;
			}
			unsigned int padding = (static_cast<int>(simdWidth) - (*setIt % static_cast<int>(simdWidth))) % static_cast<int>(simdWidth);
			for(unsigned int i = 0; i < padding; i++)
			{
				mPaddedTriplets.pushBack(paddingDummy);
			}
		}
	}
}

namespace
{
struct TripletSet
{
	TripletSet() : mMark(0xFFFFFFFF)
	{
		mNumReplays[0] = mNumReplays[1] = mNumReplays[2] = 1;
		memset(mNumConflicts[0], 0, 32);
		memset(mNumConflicts[1], 0, 32);
		memset(mNumConflicts[2], 0, 32);
	}

	uint32_t mMark; // triplet index

	// [3] because each particle is fetched on a different instruction.
	uint8_t mNumReplays[3]; //how many times the instruction needs to be executed to resolve all bank conflicts (= maxElement(mNumConflicts[3][0...31]))
	uint8_t mNumConflicts[3][32]; //Number of accesses for each of the 32 banks (if it is 1 then there is no conflict on that bank)
};

/*
struct GreaterSum
{
    typedef cloth::Vector<uint32_t>::Type Container;

    GreaterSum(const Container& cont)
        : mContainer(cont)
    {}

    bool operator()(const cloth::Vec4u& a, const cloth::Vec4u& b) const
    {
        return mContainer[a.x] + mContainer[a.y] + mContainer[a.z]
            > mContainer[b.x] + mContainer[b.y] + mContainer[b.z];
    }

    const Container& mContainer;
};
*/

// calculate the inclusive prefix sum, equivalent of std::partial_sum
template <typename Iter>
void prefixSum(Iter first, Iter last, Iter dest)
{
	if (first == last)
		return;
	else
	{
		*(dest++) = *(first++);

		for (; first != last; ++first, ++dest)
			*dest = *(dest - 1) + *first;
	}
}

class AdjacencyQuerier
{
private:
	typedef nv::cloth::Vector<uint32_t>::Type UintVector;
	typedef nv::cloth::Vector<uint32_t>::Type::Iterator UintVectorIterator;
	typedef nv::cloth::Vector<nv::cloth::Vec4u>::Type::ConstIterator ConstTripletIter;
	UintVector mAdjacencyIndecies;
	UintVector mAdjacencies;
	uint32_t mMaxAdjacentCount;

public:
	AdjacencyQuerier(nv::cloth::Vector<nv::cloth::Vec4u>::Type const& triplets, int particleCount)
	{
		{
			UintVector& adjacencyCount = mAdjacencyIndecies; //use same memory as mAdjacencyIndecies
			adjacencyCount.resize(particleCount+1, uint32_t(0)); //initialize count to 0. Size+1 used later for prefixSum/indices

			// count the number of triplets per particle
			for(ConstTripletIter tIt = triplets.begin(); tIt != triplets.end(); ++tIt)
			{
				for(int i = 0; i < 3; i++) //for each index in the triplet
				{
					const uint32_t particleIndex = (*tIt)[i];
					adjacencyCount[particleIndex] += 1;
				}
			}
			//adjacentcyCount[i] now holds the number of triplets referring to particle i

			mMaxAdjacentCount = *physx::shdfnd::maxElement(adjacencyCount.begin(), adjacencyCount.end());

			// compute in place prefix sum (inclusive)
			prefixSum(adjacencyCount.begin(), adjacencyCount.end(), mAdjacencyIndecies.begin());
			// we use mAdjacencyIndecies from now on
		}

		mAdjacencies.resize(mAdjacencyIndecies.back());
		uint32_t tripletIndex = 0;
		for(ConstTripletIter tIt = triplets.begin(); tIt != triplets.end(); ++tIt, ++tripletIndex)
		{
			for(int i = 0; i < 3; i++) //for each index in the triplet
			{
				const uint32_t particleIndex = (*tIt)[i];

				//Decrement mAdjacencyIndecies here to convert it from inclusive to exclusive prefix sum
				const uint32_t adjacencyIndex = --mAdjacencyIndecies[particleIndex];

				mAdjacencies[adjacencyIndex] = tripletIndex;
			}
		}
		//mAdjacencies[mAdjacencyIndecies[i]] now stores the first triplet index referring to particle i
		//mAdjacencies[mAdjacencyIndecies[i+1]] stores one past the last
	}

	uint32_t getMaxAdjacentCount() const
	{ 
		return mMaxAdjacentCount;
	}

	nv::cloth::Range<const uint32_t> getAdjacentTriplets(uint32_t particleIndex) const
	{
		//use .begin() + offset to get around bounds check
		auto begin = &*(mAdjacencies.begin() + mAdjacencyIndecies[particleIndex]);
		auto end = &*(mAdjacencies.begin() + mAdjacencyIndecies[particleIndex + 1]);
		return nv::cloth::Range<const uint32_t>(begin, end);
	}
};

}

// CUDA version. Doesn't add padding, optimizes for bank conflicts
void cloth::TripletScheduler::warp(uint32_t numParticles, uint32_t warpWidth)
{
	// warpWidth has to be <= 32 and a power of two
	NV_CLOTH_ASSERT(warpWidth == 32 || warpWidth == 16);

	if (mTriplets.empty())
		return;

	AdjacencyQuerier adjacencyQuerier(mTriplets, numParticles);

	uint32_t warpMask = warpWidth - 1;

	uint32_t numSets = adjacencyQuerier.getMaxAdjacentCount(); // start with minimum number of sets
	Vector<TripletSet>::Type sets(numSets);

	// maps triplets to the set index that contains the triplet (setIndex = setIndices[tripletIndex])
	Vector<uint32_t>::Type setIndices(mTriplets.size(), uint32_t(-1));

	mSetSizes.resize(numSets);

	uint32_t tripletIndex = 0;
	TripletIter tIt, tEnd = mTriplets.end();

	// color triplets (assign to sets)
	for (tIt = mTriplets.begin(); tIt != tEnd; ++tIt, ++tripletIndex)
	{
		// mark sets with adjacent triplets
		for (int i = 0; i < 3; ++i)
		{
			uint32_t particleIndex = (*tIt)[i];
			Range<const uint32_t> adjacentTriplets = adjacencyQuerier.getAdjacentTriplets(particleIndex);
			
			//for all triplets adjacent to (sharing) particle 'particleIndex'
			for (; adjacentTriplets.begin() != adjacentTriplets.end(); adjacentTriplets.popFront())
			{
				uint32_t setIndex;
				if(numSets > (setIndex = setIndices[adjacentTriplets.front()]))
				{
					//the set 'setIndex' has an adjacent triplet in it (sharing at least one vertex with tripletIndex)
					sets[setIndex].mMark = tripletIndex;
				}
			}
		}

		// find valid set with smallest number of additional replays when adding triplet tIt
		uint32_t bestIndex = numSets;
		uint32_t minAdditionalReplays = 4; // one more than the maximum possible (for 3 particles)
		for (uint32_t setIndex = 0; setIndex < numSets && minAdditionalReplays; ++setIndex)
		{
			const TripletSet& set = sets[setIndex];

			if (set.mMark == tripletIndex)
				continue; // triplet collision (this set contains an adjacent triplet)

			// count additional replays caused by inserting triplet 'tIt' to in set 'setIndex'
			uint32_t numReplays = 0;
			for(uint32_t i = 0; i < 3; ++i) //for each particle in the triplet
			{
				const uint32_t particleIndex = (*tIt)[i];
				const uint32_t bank = warpMask & particleIndex;

				// an additional replay will only be caused if the additional bank conflict is
				//  on a bank that is the current bottleneck
				numReplays += set.mNumReplays[i] == set.mNumConflicts[i][bank];
			}

			// hold on to this set if it has less additional replays compared to our current best set
			if (minAdditionalReplays > numReplays)
			{
				minAdditionalReplays = numReplays;
				bestIndex = setIndex;
			}
		}

		// add new set if none found
		if (bestIndex == numSets)
		{
			sets.pushBack(TripletSet());
			mSetSizes.pushBack(0);
			++numSets;
		}

		// increment mNumReplays or reset if warp filled
		TripletSet& set = sets[bestIndex];
		if (++mSetSizes[bestIndex] & warpMask)
		{
			for (uint32_t i = 0; i < 3; ++i)
			{
				const uint32_t particleIndex = (*tIt)[i];
				const uint32_t bank = warpMask & particleIndex;
				set.mNumReplays[i] = std::max(set.mNumReplays[i], ++set.mNumConflicts[i][bank]);
			}
		}
		else
		{ 
			set = TripletSet();
		}

		setIndices[tripletIndex] = bestIndex;
	}

	// reorder triplets so that the sets are in continuous memory
	Vector<uint32_t>::Type setOffsets(mSetSizes.size());
	prefixSum(mSetSizes.begin(), mSetSizes.end(), setOffsets.begin());

	Vector<Vec4u>::Type triplets(mTriplets.size());
	Vector<uint32_t>::Type::ConstIterator iIt = setIndices.begin();
	for (tIt = mTriplets.begin(), tripletIndex = 0; tIt != tEnd; ++tIt, ++iIt)
		triplets[--setOffsets[*iIt]] = *tIt;

	mTriplets.swap(triplets);
}


/* Interface used for unit tests */
cloth::TripletSchedulerTestInterface::TripletSchedulerTestInterface(Range<const uint32_t[4]> triplets)
	:
	mScheduler(new cloth::TripletScheduler(triplets))
{}

cloth::TripletSchedulerTestInterface::~TripletSchedulerTestInterface()
{
	delete mScheduler;
}
void cloth::TripletSchedulerTestInterface::simd(uint32_t numParticles, uint32_t simdWidth)
{
	mScheduler->simd(numParticles, simdWidth);
}
void cloth::TripletSchedulerTestInterface::warp(uint32_t numParticles, uint32_t warpWidth)
{
	mScheduler->warp(numParticles, warpWidth);
}

cloth::Range<const uint32_t> cloth::TripletSchedulerTestInterface::GetTriplets()
{
	return Range<const uint32_t>(
		reinterpret_cast<uint32_t*>(mScheduler->mTriplets.begin()),
		reinterpret_cast<uint32_t*>(mScheduler->mTriplets.begin() + mScheduler->mTriplets.size())
		); 
}
cloth::Range<const uint32_t> cloth::TripletSchedulerTestInterface::GetSetSizes()
{
	return Range<const uint32_t>(
		mScheduler->mSetSizes.begin(),
		mScheduler->mSetSizes.begin() + mScheduler->mSetSizes.size()
		);
}

NV_CLOTH_API(nv::cloth::TripletSchedulerTestInterface*) NvClothCreateTripletScheduler(nv::cloth::Range<const uint32_t[4]> triplets)
{
	return new nv::cloth::TripletSchedulerTestInterface(triplets);
}
NV_CLOTH_API(void) NvClothDestroyTripletScheduler(nv::cloth::TripletSchedulerTestInterface* interface)
{
	delete interface;
}
