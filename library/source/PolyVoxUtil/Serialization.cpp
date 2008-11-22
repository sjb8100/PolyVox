#include "PolyVoxUtil/Serialization.h"

#include "PolyVoxCore/BlockVolume.h"
#include "PolyVoxCore/BlockVolumeIterator.h"
#include "PolyVoxCore/Utility.h"

using namespace std;

namespace PolyVox
{
	//Note: we don't do much error handling in here - exceptions will simply be propergated up to the caller.
	//FIXME - think about pointer ownership issues. Or could return volume by value if the copy constructor is shallow
	BlockVolume<uint8>* loadVolumeRaw(istream& stream)
	{
		//Read volume dimensions
		uint8 volumeWidthPower = 0;
		uint8 volumeHeightPower = 0;
		uint8 volumeDepthPower = 0;
		stream.read(reinterpret_cast<char*>(&volumeWidthPower), sizeof(volumeWidthPower));
		stream.read(reinterpret_cast<char*>(&volumeHeightPower), sizeof(volumeHeightPower));
		stream.read(reinterpret_cast<char*>(&volumeDepthPower), sizeof(volumeDepthPower));
		
		//FIXME - need to support non cubic volumes
		BlockVolume<uint8>* volume = new BlockVolume<uint8>(volumeWidthPower);

		uint16 volumeWidth = 0x0001 << volumeWidthPower;
		uint16 volumeHeight = 0x0001 << volumeHeightPower;
		uint16 volumeDepth = 0x0001 << volumeDepthPower;

		//Read data
		BlockVolumeIterator<uint8> volIter(*volume);
		for(uint16 z = 0; z < volumeDepth; ++z)
		{
			for(uint16 y = 0; y < volumeHeight; ++y)
			{
				for(uint16 x = 0; x < volumeWidth; ++x)
				{
					uint8 value = 0;
					stream.read(reinterpret_cast<char*>(&value), sizeof(value));
			
					volIter.setPosition(x,y,z);
					volIter.setVoxel(value);
				}
			}
		}

		return volume;
	}

	void saveVolumeRaw(std::ostream& stream, BlockVolume<uint8>& volume)
	{
		//Write volume dimensions
		uint16 volumeWidth = volume.getSideLength();
		uint16 volumeHeight = volume.getSideLength();
		uint16 volumeDepth  = volume.getSideLength();

		uint8 volumeWidthPower = logBase2(volumeWidth);
		uint8 volumeHeightPower = logBase2(volumeHeight);
		uint8 volumeDepthPower = logBase2(volumeDepth);

		stream.write(reinterpret_cast<char*>(&volumeWidthPower), sizeof(volumeWidthPower));
		stream.write(reinterpret_cast<char*>(&volumeHeightPower), sizeof(volumeHeightPower));
		stream.write(reinterpret_cast<char*>(&volumeDepthPower), sizeof(volumeDepthPower));

		//Write data
		BlockVolumeIterator<uint8> volIter(volume);
		for(uint16 z = 0; z < volumeDepth; ++z)
		{
			for(uint16 y = 0; y < volumeHeight; ++y)
			{
				for(uint16 x = 0; x < volumeWidth; ++x)
				{
					volIter.setPosition(x,y,z);
					uint8 value = volIter.getVoxel();
					stream.write(reinterpret_cast<char*>(&value), sizeof(value));
				}
			}
		}
	}
}