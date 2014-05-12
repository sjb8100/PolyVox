/*******************************************************************************
Copyright (c) 2010 Matt Williams

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

#include "TestSurfaceExtractor.h"

#include "PolyVoxCore/Density.h"
#include "PolyVoxCore/MaterialDensityPair.h"
#include "PolyVoxCore/SimpleVolume.h"
#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"

#include <QtTest>

using namespace PolyVox;

// Test our ability to modify the behaviour of the MarchingCubesSurfaceExtractor. This simple example only modifies
// the threshold (and actually this can be achieved by passing a parameter to the constructor of the
// DefaultMarchingCubesController) but you could implement custom behaviour in the other members
// if you wanted too. Actually, it's not clear if this ability is really useful because I can't think
// what you'd modify apart from the threshold but the ability is at least available. Also, the
// DefaultMarchingCubesController is templatised whereas this exmple shows that controllers don't
// have to be.
class CustomMarchingCubesController
{
public:
	typedef float DensityType;
	typedef float MaterialType;

	float convertToDensity(float voxel)
	{
		return voxel;
	}

	float convertToMaterial(float /*voxel*/)
	{
		return 1;
	}

	float blendMaterials(float /*a*/, float /*b*/, float /*weight*/)
	{
		return 1;
	}

	float getThreshold(void)
	{
		return 50.0f;
	}
};

// These 'writeDensityValueToVoxel' functions provide a unified interface for writting densities to primative and class voxel types.
// They are conceptually the inverse of the 'convertToDensity' function used by the MarchingCubesSurfaceExtractor. They probably shouldn't be part
// of PolyVox, but they might be usful to other tests so we cold move them into a 'Tests.h' or something in the future.
template<typename VoxelType>
void writeDensityValueToVoxel(int valueToWrite, VoxelType& voxel)
{
	voxel = valueToWrite;
}

template<>
void writeDensityValueToVoxel(int valueToWrite, Density8& voxel)
{
	voxel.setDensity(valueToWrite);
}

template<>
void writeDensityValueToVoxel(int valueToWrite, MaterialDensityPair88& voxel)
{
	voxel.setDensity(valueToWrite);
}

template<typename VoxelType>
void writeMaterialValueToVoxel(int /*valueToWrite*/, VoxelType& /*voxel*/)
{
	//Most types don't have a material
	return;
}

template<>
void writeMaterialValueToVoxel(int valueToWrite, MaterialDensityPair88& voxel)
{
	voxel.setMaterial(valueToWrite);
}

// Runs the surface extractor for a given type. 
template <typename VoxelType>
SurfaceMesh<MarchingCubesVertex<VoxelType> > testForType(void) //I think we could avoid specifying this return type by using auto/decltype?
{
	const int32_t uVolumeSideLength = 32;

	//Create empty volume
	SimpleVolume<VoxelType> volData(Region(Vector3DInt32(0,0,0), Vector3DInt32(uVolumeSideLength-1, uVolumeSideLength-1, uVolumeSideLength-1)));

	for (int32_t z = 0; z < uVolumeSideLength; z++)
	{
		for (int32_t y = 0; y < uVolumeSideLength; y++)
		{
			for (int32_t x = 0; x < uVolumeSideLength; x++)
			{
				VoxelType voxelValue;
				//Create a density field which changes throughout the volume.
				writeDensityValueToVoxel<VoxelType>(x + y + z, voxelValue);
				//Two different materials in two halves of the volume
				writeMaterialValueToVoxel<VoxelType>(z > uVolumeSideLength / 2 ? 42 : 79, voxelValue);
				volData.setVoxelAt(x, y, z, voxelValue);
			}
		}
	}

	DefaultMarchingCubesController<VoxelType> controller;
	controller.setThreshold(50);

	auto result = extractMarchingCubesSurface(&volData, volData.getEnclosingRegion(), WrapModes::Border, VoxelType(), controller);

	return result;
}

void testCustomController(SurfaceMesh<MarchingCubesVertex<float> >& result)
{
	const int32_t uVolumeSideLength = 32;

	//Create empty volume
	SimpleVolume<float> volData(Region(Vector3DInt32(0,0,0), Vector3DInt32(uVolumeSideLength-1, uVolumeSideLength-1, uVolumeSideLength-1)));

	for (int32_t z = 0; z < uVolumeSideLength; z++)
	{
		for (int32_t y = 0; y < uVolumeSideLength; y++)
		{
			for (int32_t x = 0; x < uVolumeSideLength; x++)
			{
				float voxelValue = x + y + z;
				volData.setVoxelAt(x, y, z, voxelValue);
			}
		}
	}

	CustomMarchingCubesController controller;
	MarchingCubesSurfaceExtractor< SimpleVolume<float>, CustomMarchingCubesController > extractor(&volData, volData.getEnclosingRegion(), &result, WrapModes::Border, 0, controller);
	extractor.execute();
}

void TestSurfaceExtractor::testExecute()
{
	const static uint32_t uExpectedVertices = 4731;
	const static uint32_t uExpectedIndices = 12810;
	const static uint32_t uMaterialToCheck = 3000;
	const static float fExpectedMaterial = 42.0f;
	const static float fNoMaterial = 1.0f;

	SurfaceMesh<MarchingCubesVertex<int8_t> > mesh;
	//Run the test for various voxel types.
	QBENCHMARK {
		mesh = testForType<int8_t>();
	}
	QCOMPARE(mesh.getNoOfVertices(), uExpectedVertices);
	QCOMPARE(mesh.getNoOfIndices(), uExpectedIndices);
	QCOMPARE(mesh.getVertices()[uMaterialToCheck].getMaterial(), static_cast<int8_t>(fNoMaterial));

	auto mesh1 = testForType<uint8_t>();
	QCOMPARE(mesh1.getNoOfVertices(), uExpectedVertices);
	QCOMPARE(mesh1.getNoOfIndices(), uExpectedIndices);
	QCOMPARE(mesh1.getVertices()[uMaterialToCheck].getMaterial(), static_cast<uint8_t>(fNoMaterial));

	auto mesh2 = testForType<int16_t>();
	QCOMPARE(mesh2.getNoOfVertices(), uExpectedVertices);
	QCOMPARE(mesh2.getNoOfIndices(), uExpectedIndices);
	QCOMPARE(mesh2.getVertices()[uMaterialToCheck].getMaterial(), static_cast<int16_t>(fNoMaterial));

	auto mesh3 = testForType<uint16_t>();
	QCOMPARE(mesh3.getNoOfVertices(), uExpectedVertices);
	QCOMPARE(mesh3.getNoOfIndices(), uExpectedIndices);
	QCOMPARE(mesh3.getVertices()[uMaterialToCheck].getMaterial(), static_cast<uint16_t>(fNoMaterial));

	auto mesh4 = testForType<int32_t>();
	QCOMPARE(mesh4.getNoOfVertices(), uExpectedVertices);
	QCOMPARE(mesh4.getNoOfIndices(), uExpectedIndices);
	QCOMPARE(mesh4.getVertices()[uMaterialToCheck].getMaterial(), static_cast<int32_t>(fNoMaterial));

	auto mesh5 = testForType<uint32_t>();
	QCOMPARE(mesh5.getNoOfVertices(), uExpectedVertices);
	QCOMPARE(mesh5.getNoOfIndices(), uExpectedIndices);
	QCOMPARE(mesh5.getVertices()[uMaterialToCheck].getMaterial(), static_cast<uint32_t>(fNoMaterial));

	auto mesh6 = testForType<float>();
	QCOMPARE(mesh6.getNoOfVertices(), uExpectedVertices);
	QCOMPARE(mesh6.getNoOfIndices(), uExpectedIndices);
	QCOMPARE(mesh6.getVertices()[uMaterialToCheck].getMaterial(), static_cast<float>(fNoMaterial));

	auto mesh7 = testForType<double>();
	QCOMPARE(mesh7.getNoOfVertices(), uExpectedVertices);
	QCOMPARE(mesh7.getNoOfIndices(), uExpectedIndices);
	QCOMPARE(mesh7.getVertices()[uMaterialToCheck].getMaterial(), static_cast<double>(fNoMaterial));

	auto mesh8 = testForType<Density8>();
	QCOMPARE(mesh8.getNoOfVertices(), uExpectedVertices);
	QCOMPARE(mesh8.getNoOfIndices(), uExpectedIndices);
	QCOMPARE(mesh8.getVertices()[uMaterialToCheck].getMaterial(), static_cast<Density8>(fNoMaterial));

	auto mesh9 = testForType<MaterialDensityPair88>();
	QCOMPARE(mesh9.getNoOfVertices(), uExpectedVertices);
	QCOMPARE(mesh9.getNoOfIndices(), uExpectedIndices);
	//QCOMPARE(mesh9.getVertices()[uMaterialToCheck].getMaterial(), fExpectedMaterial);

	//Test whether the CustomSurfaceExtractor works.
	/*testCustomController(floatMesh);
	QCOMPARE(floatMesh.getNoOfVertices(), uExpectedVertices);
	QCOMPARE(floatMesh.getNoOfIndices(), uExpectedIndices);
	QCOMPARE(floatMesh.getVertices()[uMaterialToCheck].getMaterial(), fNoMaterial);*/
}

QTEST_MAIN(TestSurfaceExtractor)
