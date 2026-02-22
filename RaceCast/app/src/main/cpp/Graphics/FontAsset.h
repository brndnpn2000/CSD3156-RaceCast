#ifndef FONTASSET_H
#define FONTASSET_H

#include <GLES3/gl31.h>
#include <unordered_map>
#include <string>
#include <android/asset_manager.h>

struct Glyph
{
    float advanceX;         // how far to move cursor after this char
    float width, height;    // size of the glyph bitmap
    float bearingX;         // left offset from cursor
    float bearingY;         // top offset from baseline
    float uv[4][2];         // TL, BL, BR, TR (matches your TextureCoordinate layout)
};

class FontAsset
{
public:
    GLuint m_textureID = 0;
    int    m_atlasW = 0, m_atlasH = 0;
    std::unordered_map<char, Glyph> m_glyphs;

    bool Load(AAssetManager* mgr, const std::string& assetPath);
    void Unload();
};

#endif