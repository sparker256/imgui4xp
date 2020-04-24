/*
 * ImgFontAtlas.cpp
 *
 * Integration for dear imgui into X-Plane.
 *
 * Copyright (C) 2020, Christopher Collins
*/

#include "ImgFontAtlas.h"
#include <XPLMGraphics.h>

ImgFontAtlas::ImgFontAtlas():
    mOurAtlas(nullptr),
    mTextureBound(false),
    mGLTextureNum(0)
{
    mOurAtlas = new ImFontAtlas;
}

ImgFontAtlas::~ImgFontAtlas()
{
    if (mTextureBound) {
        GLuint glTexNum = mGLTextureNum;
        glDeleteTextures(1, &glTexNum);
        mTextureBound = false;
    }
    delete mOurAtlas;
    mOurAtlas = nullptr;
}

ImFont *
ImgFontAtlas::AddFont(const ImFontConfig *font_cfg)
{
    return mOurAtlas->AddFont(font_cfg);
}

ImFont *
ImgFontAtlas::AddFontDefault(const ImFontConfig *font_cfg)
{
    return mOurAtlas->AddFontDefault(font_cfg);
}

ImFont *
ImgFontAtlas::AddFontFromFileTTF(const char *filename,
                                 float size_pixels,
                                 const ImFontConfig *font_cfg,
                                 const unsigned short *glyph_ranges)
{
    return mOurAtlas->AddFontFromFileTTF(filename, size_pixels, font_cfg, glyph_ranges);
}

ImFont *
ImgFontAtlas::AddFontFromMemoryTTF(void *font_data,
                                   int font_size,
                                   float size_pixels,
                                   const ImFontConfig *font_cfg,
                                   const unsigned short *glyph_ranges)
{
    return mOurAtlas->AddFontFromMemoryTTF(font_data, font_size, size_pixels, font_cfg, glyph_ranges);
}

ImFont *
ImgFontAtlas::AddFontFromMemoryCompressedTTF(const void *compressed_font_data,
                                             int compressed_font_size,
                                             float size_pixels,
                                             const ImFontConfig *font_cfg,
                                             const unsigned short *glyph_ranges)
{
    return mOurAtlas->AddFontFromMemoryCompressedTTF(compressed_font_data, compressed_font_size, size_pixels, font_cfg, glyph_ranges);
}

ImFont *
ImgFontAtlas::AddFontFromMemoryCompressedBase85TTF(const char *compressed_font_data_base85,
                                                   float size_pixels,
                                                   const ImFontConfig *font_cfg,
                                                   const unsigned short *glyph_ranges)
{
    return mOurAtlas->AddFontFromMemoryCompressedBase85TTF(compressed_font_data_base85, size_pixels, font_cfg, glyph_ranges);
}

ImFontAtlas *
ImgFontAtlas::getAtlas()
{
    return mOurAtlas;
}

void
ImgFontAtlas::bindTexture()
{
    if (mTextureBound)
        return;

    XPLMGenerateTextureNumbers(&mGLTextureNum, 1);

    unsigned char *pixData = nullptr;
    int width, height;
    mOurAtlas->GetTexDataAsRGBA32(&pixData, &width, &height);

    XPLMBindTexture2d(mGLTextureNum, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixData);

    mOurAtlas->SetTexID((void *)((intptr_t)mGLTextureNum));
    mTextureBound = true;
}
