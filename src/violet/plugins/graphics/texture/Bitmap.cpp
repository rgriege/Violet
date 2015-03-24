#include "violet/plugins/graphics/texture/Bitmap.h"

#include <fstream>
#include <iostream>

using namespace Violet;

namespace BitmapNamespace
{
	struct DibHeader
	{
		DibHeader();

		uint32 size;
		int32 width;
		int32 height;
		uint16 colorPlaneCount;
		uint16 bitsPerPixel;
		uint32 compressionType;
		uint32 imageSize;
		int32 horizontalResolution;
		int32 verticalResolucation;
		uint32 colorCount;
		uint32 colorsUsedCount;
	};

	template <typename T>
	T read(void *& buffer)
	{
		T *& typedBuffer = reinterpret_cast<T*&>(buffer);
		const T result = *typedBuffer;
		++typedBuffer;
		return result;
	}
};

using namespace BitmapNamespace;

std::unique_ptr<Bitmap> Bitmap::load(const char * filename)
{
	std::ifstream stream(filename, std::ios_base::binary);
	if (!stream)
	{
		std::cout << "Could not open bitmap file " << filename << std::endl;
		return nullptr;
	}

	return load(stream);
}



std::unique_ptr<Bitmap> Bitmap::load(std::istream & stream)
{
	char buffer[40];

	// file header
	stream.read(buffer, 14);

	std::string headers[] = { "BM", "BA", "CI", "CP", "IC", "PT" };
	if (std::find(std::begin(headers), std::end(headers), std::string(buffer, 2)) == std::end(headers))
	{
		std::cout << "Incorrect bitmap header " << std::endl;
		return nullptr;
	}

	const uint32 fileSize = *reinterpret_cast<uint32*>(buffer + 2);
	const uint32 pixelArrayIndex = *reinterpret_cast<uint32*>(buffer + 10);

	// DIB header
	DibHeader dibHeader;
	stream.read(buffer, 4);
	dibHeader.size = *reinterpret_cast<uint32*>(buffer);
	stream.read(buffer, dibHeader.size - 4);
	void * bufPtr = buffer;
	switch (dibHeader.size)
	{
	case 12: // BITMAPCOREHEADER or OS21XBITMAPHEADER
		dibHeader.width = read<uint16>(bufPtr);
		dibHeader.height = read<uint16>(bufPtr);
		dibHeader.colorPlaneCount = read<uint16>(bufPtr);
		if (dibHeader.colorPlaneCount != 1)
		{
			std::cout << "Incorrect color plane count: " << dibHeader.colorPlaneCount << std::endl;
			return nullptr;
		}
		dibHeader.bitsPerPixel = read<uint16>(bufPtr);
		break;

	case 40: // BITMAPINFOHEADER
		dibHeader.width = read<int32>(bufPtr);
		dibHeader.height = read<int32>(bufPtr);
		dibHeader.colorPlaneCount = read<uint16>(bufPtr);
		if (dibHeader.colorPlaneCount != 1)
		{
			std::cout << "Incorrect color plane count: " << dibHeader.colorPlaneCount << std::endl;
			return nullptr;
		}
		dibHeader.bitsPerPixel = read<uint16>(bufPtr);
		dibHeader.compressionType = read<uint32>(bufPtr);
		if (dibHeader.compressionType != 0)
		{
			std::cout << "Unsupported compression type: " << dibHeader.compressionType << std::endl;
			return nullptr;
		}
		dibHeader.imageSize = read<uint32>(bufPtr); // can be dummy (0) for BI_RGB (0) compression
		dibHeader.horizontalResolution = read<int32>(bufPtr);
		dibHeader.verticalResolucation = read<int32>(bufPtr);
		dibHeader.colorCount = read<uint32>(bufPtr);
		dibHeader.colorsUsedCount = read<uint32>(bufPtr); // generally ignored
		break;

	case 52: // BITMAPV2INFOHEADER
	case 56: // BITMAPV3INFOHEADER
	case 64: // OS22XBITMAPHEADER
	case 108: // BITMAPV4HEADER
	case 124: // BITMAPV5HEADER
		std::cout << "Unsupported header type: " << dibHeader.size << std::endl;
		return nullptr;
	}

	const uint32 colorTableSize = dibHeader.colorsUsedCount != 0 ? dibHeader.colorsUsedCount : static_cast<uint32>(pow(2, dibHeader.bitsPerPixel));

	return nullptr;
}

BitmapNamespace::DibHeader::DibHeader() :
	size(0),
	width(0),
	height(0),
	colorPlaneCount(0),
	bitsPerPixel(0),
	compressionType(0),
	imageSize(0),
	horizontalResolution(0),
	verticalResolucation(0),
	colorCount(0),
	colorsUsedCount(0)
{
}