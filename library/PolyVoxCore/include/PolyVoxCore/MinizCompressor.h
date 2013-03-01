#ifndef __PolyVox_MinizCompressor_H__
#define __PolyVox_MinizCompressor_H__

#include "PolyVoxCore/Compressor.h"

namespace PolyVox
{
	class MinizCompressor : public Compressor
	{
	public:
		MinizCompressor(int iCompressionLevel = 6); // Miniz defines MZ_DEFAULT_LEVEL = 6 so we use the same here
		~MinizCompressor();

		uint32_t getMaxCompressedSize(uint32_t uUncompressedInputSize);
		uint32_t compress(void* pSrcData, uint32_t uSrcLength, void* pDstData, uint32_t uDstLength);
		uint32_t decompress(void* pSrcData, uint32_t uSrcLength, void* pDstData, uint32_t uDstLength);
	
	private:
		unsigned int m_uCompressionFlags;

		// tdefl_compressor contains all the state needed by the low-level compressor so it's a pretty big struct (~300k).
		// We're storing it by void* because miniz does not supply a header and we don't want to include the .c file from 
		// here as it will cause linker problems.
		void* m_pDeflator;
	};
}

#endif //__PolyVox_MinizCompressor_H__