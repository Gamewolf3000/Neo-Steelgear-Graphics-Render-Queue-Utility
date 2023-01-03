#include "TextureHelper.h"

#include <cstdint>
#include <limits>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#undef max

template<typename T>
T CalculateChannelAverage(T v11, T v12, T v21, T v22)
{
	T maxValue = std::numeric_limits<T>::max();

	double a = static_cast<float>(v11) / maxValue;
	double b = static_cast<float>(v12) / maxValue;
	double c = static_cast<float>(v21) / maxValue;
	double d = static_cast<float>(v22) / maxValue;

	double average = (a + b + c + d) / 4;
	return static_cast<T>(average < 1.0 ? average * maxValue : maxValue);
}

template<typename T>
void GenerateMipMaps(std::vector<std::vector<T>>& mipmaps,
	int mipmapWidth, int mipmapHeight, int currentLevel, int maxLevel,
	int channels)
{
	T* original = mipmaps[currentLevel - 1].data();

	mipmaps[currentLevel].resize(mipmapWidth * mipmapHeight * channels);
	int originalWidth = mipmapWidth * 2;

	for (int h = 0; h < mipmapHeight; ++h)
	{
		for (int w = 0; w < mipmapWidth; ++w)
		{
			int currentMipMapPixelStart = (h * mipmapWidth + w) * channels;
			int currentOriginalX = w * 2;
			int currentOriginalY = h * 2;
			int currentOriginalPixelStart =
				(currentOriginalY * originalWidth + currentOriginalX) * channels;
			int i11 = currentOriginalPixelStart;
			int i12 = currentOriginalPixelStart + channels;
			int i21 = currentOriginalPixelStart + originalWidth * channels;
			int i22 = currentOriginalPixelStart + channels +
				originalWidth * channels;

			for (int c = 0; c < channels; ++c)
			{
				T v11 = original[i11 + c];
				T v12 = original[i12 + c];
				T v21 = original[i21 + c];
				T v22 = original[i22 + c];

				mipmaps[currentLevel][currentMipMapPixelStart + c] =
					CalculateChannelAverage(v11, v12, v21, v22);
			}
		}
	}

	if (currentLevel < maxLevel)
	{
		GenerateMipMaps(mipmaps, mipmapWidth / 2, mipmapHeight / 2,
			currentLevel + 1, maxLevel, channels);
	}
}

template<typename T>
CategoryResourceIdentifier CreateTexture(int width, int height,
	int channels, T* originalData, bool generateMipMaps, 
	Renderer<2>& renderer, const CategoryIdentifier& categoryIdentifier)
{
	CategoryResourceIdentifier toReturn;

	if (generateMipMaps == true)
	{
		int minDimension = std::min<int>(width, height);
		std::uint8_t maxMipLevel = static_cast<std::uint8_t>(std::log2(minDimension));

		std::vector<std::vector<T>> mipLevelData;
		mipLevelData.resize(maxMipLevel + 1);
		mipLevelData[0].resize(width * height * channels);
		memcpy(mipLevelData[0].data(), originalData, width * height * channels);
		GenerateMipMaps(mipLevelData, width / 2, height / 2, 1, maxMipLevel, channels);

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

	return toReturn;
}

CategoryResourceIdentifier LoadTextureStandard(const std::string & filePath,
	Renderer<2>& renderer, const CategoryIdentifier& categoryIdentifier,
	bool generateMipMaps)
{
	const int CHANNELS = 4;
	int width = 0;
	int height = 0;
	CategoryResourceIdentifier toReturn;
	
	unsigned char* originalData = 
		stbi_load(filePath.c_str(), &width, &height, nullptr, CHANNELS);

	CreateTexture(width, height, CHANNELS, originalData, generateMipMaps,
		renderer, categoryIdentifier);

	STBI_FREE(originalData);
	return toReturn;
}

CategoryResourceIdentifier LoadTextureHDR(const std::string& filePath, 
	Renderer<2>& renderer, const CategoryIdentifier& categoryIdentifier,
	bool generateMipMaps)
{
	const int CHANNELS = 3;
	int width = 0;
	int height = 0;
	CategoryResourceIdentifier toReturn;

	float* originalData = 
		stbi_loadf(filePath.c_str(), &width, &height, nullptr, CHANNELS);

	CreateTexture(width, height, CHANNELS, originalData, generateMipMaps,
		renderer, categoryIdentifier);

	STBI_FREE(originalData);
	return toReturn;
}




//unsigned char CalculateChannelAverage(unsigned char v11,
//	unsigned char v12, unsigned char v21, unsigned char v22)
//{
//	float a = v11 / 255.0f;
//	float b = v12 / 255.0f;
//	float c = v21 / 255.0f;
//	float d = v22 / 255.0f;
//
//	float average = (a + b + c + d) / 4;
//	return static_cast<unsigned char>(average < 1.0f ? average * 255 : 255);
//}
//
//void GenerateMipMaps(std::vector<std::vector<unsigned char>>& mipmaps,
//	int mipmapWidth, int mipmapHeight, int currentLevel, int maxLevel)
//{
//	const int CHANNELS = 4;
//	unsigned char* original = mipmaps[currentLevel - 1].data();
//
//	mipmaps[currentLevel].resize(mipmapWidth * mipmapHeight * CHANNELS);
//	int originalWidth = mipmapWidth * 2;
//
//	for (int h = 0; h < mipmapHeight; ++h)
//	{
//		for (int w = 0; w < mipmapWidth; ++w)
//		{
//			int currentMipMapPixelStart = (h * mipmapWidth + w) * CHANNELS;
//			int currentOriginalX = w * 2;
//			int currentOriginalY = h * 2;
//			int currentOriginalPixelStart = 
//				(currentOriginalY * originalWidth + currentOriginalX) * CHANNELS;
//			int i11 = currentOriginalPixelStart;
//			int i12 = currentOriginalPixelStart + CHANNELS;
//			int i21 = currentOriginalPixelStart + originalWidth * CHANNELS;
//			int i22 = currentOriginalPixelStart + CHANNELS +
//				originalWidth * CHANNELS;
//
//			for (int c = 0; c < CHANNELS; ++c)
//			{
//				unsigned char v11 = original[i11 + c];
//				unsigned char v12 = original[i12 + c];
//				unsigned char v21 = original[i21 + c];
//				unsigned char v22 = original[i22 + c];
//
//				mipmaps[currentLevel][currentMipMapPixelStart + c] =
//					CalculateChannelAverage(v11, v12, v21, v22);
//			}
//		}
//	}
//
//	if (currentLevel < maxLevel)
//	{
//		GenerateMipMaps(mipmaps, mipmapWidth / 2, mipmapHeight / 2,
//			currentLevel + 1, maxLevel);
//	}
//}

//CategoryResourceIdentifier LoadTextureStandard(const std::string& filePath,
//	Renderer<2>& renderer, const CategoryIdentifier& categoryIdentifier,
//	bool generateMipMaps)
//{
//	const int CHANNELS = 4;
//	int width = 0;
//	int height = 0;
//	CategoryResourceIdentifier toReturn;
//
//	unsigned char* originalData =
//		stbi_load(filePath.c_str(), &width, &height, nullptr, CHANNELS);
//
//	if (generateMipMaps == true)
//	{
//		int minDimension = std::min<int>(width, height);
//		std::uint8_t maxMipLevel = static_cast<std::uint8_t>(std::log2(minDimension));
//
//		std::vector<std::vector<unsigned char>> mipLevelData;
//		mipLevelData.resize(maxMipLevel + 1);
//		mipLevelData[0].resize(width * height * CHANNELS);
//		memcpy(mipLevelData[0].data(), originalData, width * height * CHANNELS);
//		GenerateMipMaps(mipLevelData, width / 2, height / 2, 1, maxMipLevel, CHANNELS);
//
//		toReturn = renderer.ResourceCategories().CreateTexture2D(
//			categoryIdentifier, width, height, 1, maxMipLevel + 1);
//
//		for (std::uint8_t i = 0; i < maxMipLevel; ++i)
//		{
//			renderer.ResourceCategories().SetResourceData(toReturn,
//				mipLevelData[i].data(), i);
//		}
//	}
//	else
//	{
//		toReturn = renderer.ResourceCategories().CreateTexture2D(
//			categoryIdentifier, width, height);
//		renderer.ResourceCategories().SetResourceData(toReturn, originalData);
//	}
//
//	STBI_FREE(originalData);
//	return toReturn;
//}