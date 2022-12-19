#include "TextureHelper.h"

#include <cstdint>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned char CalculateChannelAverage(unsigned char v11,
	unsigned char v12, unsigned char v21, unsigned char v22)
{
	float a = v11 / 255.0f;
	float b = v12 / 255.0f;
	float c = v21 / 255.0f;
	float d = v22 / 255.0f;

	float average = (a + b + c + d) / 4;
	return static_cast<unsigned char>(average < 1.0f ? average * 255 : 255);
}

void GenerateMipMaps(std::vector<std::vector<unsigned char>>& mipmaps,
	int mipmapWidth, int mipmapHeight, int currentLevel, int maxLevel)
{
	const int CHANNELS = 4;
	unsigned char* original = mipmaps[currentLevel - 1].data();

	mipmaps[currentLevel].resize(mipmapWidth * mipmapHeight * CHANNELS);
	int originalWidth = mipmapWidth * 2;

	for (int h = 0; h < mipmapHeight; ++h)
	{
		for (int w = 0; w < mipmapWidth; ++w)
		{
			int currentMipMapPixelStart = (h * mipmapWidth + w) * CHANNELS;
			int currentOriginalX = w * 2;
			int currentOriginalY = h * 2;
			int currentOriginalPixelStart = 
				(currentOriginalY * originalWidth + currentOriginalX) * CHANNELS;
			int i11 = currentOriginalPixelStart;
			int i12 = currentOriginalPixelStart + CHANNELS;
			int i21 = currentOriginalPixelStart + originalWidth * CHANNELS;
			int i22 = currentOriginalPixelStart + CHANNELS +
				originalWidth * CHANNELS;

			for (int c = 0; c < CHANNELS; ++c)
			{
				unsigned char v11 = original[i11 + c];
				unsigned char v12 = original[i12 + c];
				unsigned char v21 = original[i21 + c];
				unsigned char v22 = original[i22 + c];

				mipmaps[currentLevel][currentMipMapPixelStart + c] =
					CalculateChannelAverage(v11, v12, v21, v22);
			}
		}
	}

	if (currentLevel < maxLevel)
	{
		GenerateMipMaps(mipmaps, mipmapWidth / 2, mipmapHeight / 2,
			currentLevel + 1, maxLevel);
	}
}

CategoryResourceIdentifier LoadTexture(const std::string & filePath,
	Renderer<2>& renderer, const CategoryIdentifier& categoryIdentifier,
	bool generateMipMaps)
{
	const int CHANNELS = 4;
	int width = 0;
	int height = 0;
	CategoryResourceIdentifier toReturn;
	
	unsigned char* originalData = 
		stbi_load(filePath.c_str(), &width, &height, nullptr, CHANNELS);

	if (generateMipMaps == true)
	{
		int minDimension = std::min<int>(width, height);
		std::uint8_t maxMipLevel = static_cast<std::uint8_t>(std::log2(minDimension));

		std::vector<std::vector<unsigned char>> mipLevelData;
		mipLevelData.resize(maxMipLevel + 1);
		mipLevelData[0].resize(width * height * CHANNELS);
		memcpy(mipLevelData[0].data(), originalData, width * height * CHANNELS);
		GenerateMipMaps(mipLevelData, width / 2, height / 2, 1, maxMipLevel);

		toReturn = renderer.ResourceCategories().CreateTexture2D(
			categoryIdentifier, width, height, 1, maxMipLevel + 1);

		for (std::uint8_t i = 0; i < maxMipLevel; ++i)
		{
			renderer.ResourceCategories().SetResourceData(toReturn,
				mipLevelData[i].data(), i);
		}
	}
	else
	{
		toReturn = renderer.ResourceCategories().CreateTexture2D(
			categoryIdentifier, width, height);
		renderer.ResourceCategories().SetResourceData(toReturn, originalData);
	}

	STBI_FREE(originalData);
	return toReturn;
}