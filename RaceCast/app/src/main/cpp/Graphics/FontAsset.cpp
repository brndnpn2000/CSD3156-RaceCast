#include "FontAsset.h"
#include <android/log.h>
#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "FontAsset", __VA_ARGS__)

bool FontAsset::Load(AAssetManager* mgr, const std::string& assetPath)
{
    // --- 1. Read font file from assets ---
    AAsset* asset = AAssetManager_open(mgr, assetPath.c_str(), AASSET_MODE_BUFFER);
    if (!asset) {
        LOGI("Failed to open font: %s", assetPath.c_str());
        return false;
    }
    long size = AAsset_getLength(asset);
    std::vector<unsigned char> fontBuffer(size);
    AAsset_read(asset, fontBuffer.data(), size);
    AAsset_close(asset);

    // --- 2. Init FreeType ---
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) { LOGI("FT_Init failed"); return false; }

    FT_Face face;
    if (FT_New_Memory_Face(ft, fontBuffer.data(), (FT_Long)size, 0, &face)) {
        LOGI("FT_New_Memory_Face failed for: %s", assetPath.c_str());
        FT_Done_FreeType(ft);
        return false;
    }
    FT_Set_Pixel_Sizes(face, 0, 128); // Rasterize at 128px

    // --- 3. Create atlas texture (1024x1024, single channel) ---
    m_atlasW = m_atlasH = 1024;
    std::vector<unsigned char> atlasData(m_atlasW * m_atlasH, 0);

    // Pack glyphs into atlas CPU-side first
    int x_offset = 0, y_offset = 0, row_height = 0;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 32; c < 128; ++c)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;
        FT_GlyphSlot g = face->glyph;

        int bw = (int)g->bitmap.width;
        int bh = (int)g->bitmap.rows;

        // Wrap to next row if needed
        if (x_offset + bw >= m_atlasW) {
            x_offset = 0;
            y_offset += row_height;
            row_height = 0;
        }
        row_height = std::max(row_height, bh);

        // Copy glyph bitmap into atlas buffer
        for (int row = 0; row < bh; ++row) {
            memcpy(
                    atlasData.data() + (y_offset + row) * m_atlasW + x_offset,
                    g->bitmap.buffer + row * bw,
                    bw
            );
        }

        // Store glyph metrics (divided by 100 to normalize)
        Glyph glyph{};
        glyph.advanceX = (float)(g->advance.x >> 6) / 100.f;
        glyph.width    = (float)bw / 100.f;
        glyph.height   = (float)bh / 100.f;
        glyph.bearingX = (float)g->bitmap_left  / 100.f;
        glyph.bearingY = (float)g->bitmap_top   / 100.f;

        // UVs: TL, BL, BR, TR  (matches your TextureCoordinate winding)
        float u0 = (float)x_offset        / m_atlasW;
        float v0 = (float)y_offset        / m_atlasH; // top in atlas
        float u1 = (float)(x_offset + bw) / m_atlasW;
        float v1 = (float)(y_offset + bh) / m_atlasH; // bottom in atlas

        glyph.uv[0][0] = u0; glyph.uv[0][1] = v0; // TL
        glyph.uv[1][0] = u0; glyph.uv[1][1] = v1; // BL
        glyph.uv[2][0] = u1; glyph.uv[2][1] = v1; // BR
        glyph.uv[3][0] = u1; glyph.uv[3][1] = v0; // TR

        m_glyphs[c] = glyph;
        x_offset += bw;
    }

    // --- 4. Upload atlas to GPU ---
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_atlasW, m_atlasH,
            0, GL_LUMINANCE, GL_UNSIGNED_BYTE, atlasData.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    LOGI("Font loaded: %s", assetPath.c_str());
    return true;
}

void FontAsset::Unload()
{
    if (m_textureID) {
        glDeleteTextures(1, &m_textureID);
        m_textureID = 0;
    }
    m_glyphs.clear();
}