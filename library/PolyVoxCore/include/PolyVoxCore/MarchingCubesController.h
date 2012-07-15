/*******************************************************************************
Copyright (c) 2005-2009 David Williams

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

#ifndef __PolyVox_MarchingCubesController_H__
#define __PolyVox_MarchingCubesController_H__

#include <limits>

namespace PolyVox
{
	template<typename VoxelType>
	class MarchingCubesController
	{
	public:
		typedef VoxelType DensityType;
		typedef float MaterialType;

		MarchingCubesController(void)
		{
			m_tThreshold = ((std::numeric_limits<DensityType>::min)() + (std::numeric_limits<DensityType>::max)()) / 2;
		}

		MarchingCubesController(DensityType tThreshold)
		{
			m_tThreshold = tThreshold;
		}

		DensityType convertToDensity(VoxelType voxel)
		{
			return voxel;
		}

		MaterialType convertToMaterial(VoxelType voxel)
		{
			return 1;
		}

		DensityType getThreshold(void)
		{
			// Returns a threshold value halfway between the min and max possible values.
			return m_tThreshold;
		}

	private:
		DensityType m_tThreshold;
	};
}

#endif