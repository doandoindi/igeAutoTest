#include "AutoTestImpl.h"
#include "AutoTest.h"
#include "pyxieFios.h"
#include "pyxieTexture.h"
#include "bitmapHelper.h"
#include "SDL.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_STATIC
#include <stb_image_write.h>
using namespace pyxie;

AutoTestImpl::AutoTestImpl()
{
    pyxie::pyxieFios::Instance().MakeDir("GameLoopResults");
}

AutoTestImpl::~AutoTestImpl()
{
}

void AutoTestImpl::FinishLoop()
{
    
}

void AutoTestImpl::Screenshots(const char* name)
{
    uint8_t* data = nullptr;
	int width, height;
	pyxieTexture::ReadPixels(data, width, height);
	if (data)
	{		
		char path[64];
		sprintf(path, "GameLoopResults/%s.png", name);
		FlipRGBAY(data, width, height);
		int result = stbi_write_png(path, width, height, 4, data, 0);
		PYXIE_SAFE_FREE(data);
	}
}

const char* AutoTestImpl::GetResultPath()
{
    return "GameLoopResults/result.json";
}
