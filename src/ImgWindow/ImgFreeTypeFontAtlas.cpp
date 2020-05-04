//
// Created by chris on 3/05/2020.
//

#include "ImgFreeTypeFontAtlas.h"

#include <XPLMGraphics.h>

void
ImgFreeTypeFontAtlas::bindTexture()
{
    if (mTextureBound)
        return;

    XPLMGenerateTextureNumbers(&mGLTextureNum, 1);

    unsigned char *pixData = nullptr;
    int width, height;
    ImGuiFreeType::BuildFontAtlas(mOurAtlas, mGlobalFlags);
    mOurAtlas->GetTexDataAsRGBA32(&pixData, &width, &height);

    XPLMBindTexture2d(mGLTextureNum, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixData);

    mOurAtlas->SetTexID(reinterpret_cast<void *>(static_cast<intptr_t>(mGLTextureNum)));
    mTextureBound = true;
}

ImgFreeTypeFontAtlas::ImgFreeTypeFontAtlas(unsigned int globalFlags):
    ImgFontAtlas(),
    mGlobalFlags(globalFlags)
{
}

ImgFreeTypeFontAtlas::~ImgFreeTypeFontAtlas()
{
}
