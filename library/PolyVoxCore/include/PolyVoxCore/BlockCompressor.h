/*******************************************************************************
Copyright (c) 2005-2013 David Williams and Matthew Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution. 	
*******************************************************************************/

#ifndef __PolyVox_BlockCompressor_H__
#define __PolyVox_BlockCompressor_H__

#include "PolyVoxCore/PolyVoxForwardDeclarations.h"
#include "PolyVoxCore/CompressedBlock.h"
#include "PolyVoxCore/UncompressedBlock.h"

namespace PolyVox
{
	/**
	 * Provides an interface for performing compression of blocks.
	 */
	template <typename VoxelType>
	class BlockCompressor
	{
	public:
		BlockCompressor() {};
		virtual ~BlockCompressor() {};

		virtual void compress(UncompressedBlock<VoxelType>* pSrcBlock, CompressedBlock<VoxelType>* pDstBlock) = 0;
		virtual void decompress(CompressedBlock<VoxelType>* pSrcBlock, UncompressedBlock<VoxelType>* pDstBlock) = 0;
	};
}

#endif //__PolyVox_BlockCompressor_H__