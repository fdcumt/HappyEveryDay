#include "StbImage.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

uint8* FSTBImage::StbiLoad(const char* filename, int* x, int* y, int* channels_in_file, int desired_channels)
{
	return stbi_load(filename, x, y, channels_in_file, desired_channels);
}
