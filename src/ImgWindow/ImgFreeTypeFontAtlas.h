//
// Created by chris on 3/05/2020.
//

#ifndef XSQUAWKBOX_VATSIM_IMGFREETYPEFONTATLAS_H
#define XSQUAWKBOX_VATSIM_IMGFREETYPEFONTATLAS_H

#include "ImgFontAtlas.h"
#include "imgui_freetype.h"

class ImgFreeTypeFontAtlas: public ImgFontAtlas {
public:
    explicit ImgFreeTypeFontAtlas(unsigned int globalFlags = 0);
    virtual ~ImgFreeTypeFontAtlas();

    void bindTexture() override;
protected:
    unsigned int mGlobalFlags;
};


#endif //XSQUAWKBOX_VATSIM_IMGFREETYPEFONTATLAS_H
