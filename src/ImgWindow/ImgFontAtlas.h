/*
 * ImgFontAtlas.h
 *
 * Integration for dear imgui into X-Plane.
 *
 * Copyright (C) 2020, Christopher Collins
*/

#ifndef IMGFONTATLAS_H
#define IMGFONTATLAS_H

#include "SystemGL.h"
#include <imgui.h>

/** Construct an empty font atlas we can use later
 *
 * This also assigns the texture name which is necessary as, again, must be done
 * in an x-plane compatible manner.
 *
 * @return an empty font atlas ready to be initialised.
 */
class ImgFontAtlas {
public:
    ImgFontAtlas();

    ~ImgFontAtlas();

    ImFont *AddFont(const ImFontConfig *font_cfg);

    ImFont *AddFontDefault(const ImFontConfig *font_cfg = NULL);

    ImFont *AddFontFromFileTTF(const char *filename,
                               float size_pixels,
                               const ImFontConfig *font_cfg = NULL,
                               const unsigned short *glyph_ranges = NULL);

    ImFont *AddFontFromMemoryTTF(void *font_data,
                                 int font_size,
                                 float size_pixels,
                                 const ImFontConfig *font_cfg = NULL,
                                 const unsigned short *glyph_ranges = NULL); // Note: Transfer ownership of 'ttf_data' to ImFontAtlas! Will be deleted after destruction of the atlas. Set font_cfg->FontDataOwnedByAtlas=false to keep ownership of your data and it won't be freed.
    ImFont *AddFontFromMemoryCompressedTTF(const void *compressed_font_data,
                                           int compressed_font_size,
                                           float size_pixels,
                                           const ImFontConfig *font_cfg = NULL,
                                           const unsigned short *glyph_ranges = NULL); // 'compressed_font_data' still owned by caller. Compress with binary_to_compressed_c.cpp.
    ImFont *AddFontFromMemoryCompressedBase85TTF(const char *compressed_font_data_base85,
                                                 float size_pixels,
                                                 const ImFontConfig *font_cfg = NULL,
                                                 const unsigned short *glyph_ranges = NULL);              // 'compressed_font_data_base85' still owned by caller. Compress with binary_to_compressed_c.cpp with -base85 parameter.

    /** bindTexture creates and binds the font texture to OpenGL, ready for use.
     *
     * This should be called after all fonts are loaded, before any rendering occurs!
     */
    void bindTexture();

    ImFontAtlas *getAtlas();
protected:
    ImFontAtlas *mOurAtlas;
private:
    bool        mTextureBound;
    int         mGLTextureNum;
};

#endif //IMGFONTATLAS_H
