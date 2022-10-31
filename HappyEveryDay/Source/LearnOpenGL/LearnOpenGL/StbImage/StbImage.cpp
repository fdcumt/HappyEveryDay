#include "StbImage.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

uint8* FSTBImage::StbiLoad(const char* filename, int* x, int* y, int* channels_in_file, int desired_channels)
{
	return stbi_load(filename, x, y, channels_in_file, desired_channels);
}

void FSTBImage::StbiImageFree(void* pData)
{
	stbi_image_free(pData);
}

void FSTBImage::SetFlipVerticallyOnLoad(bool bFlip)
{
	stbi_set_flip_vertically_on_load(bFlip); // tell stb_image.h to flip loaded texture's on the y-axis.
}
