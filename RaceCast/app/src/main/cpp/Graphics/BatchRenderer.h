#ifndef BATCHRENDERER_H
#define BATCHRENDERER_H

#include <GLES3/gl31.h>
#include <vector>
#include "ShaderManager.h"
#include "FontAsset.h"
#include <android/asset_manager.h>
#include <string>

#define MAX_TRIANGLE 100
#define VERTICES_PER_TRIANGLE 3

struct Vertex
{
    float position[3];
    float color[4];
    float texCoords[2];
    float texIndex;
};

struct TextureCoordinate
{
    float uv[4][2];

    float* GetTL() { return uv[0]; }
    float* GetBL() { return uv[1]; }
    float* GetBR() { return uv[2]; }
    float* GetTR() { return uv[3]; }

    TextureCoordinate()
    {
        // tl, bl, br, tr
        uv[0][0] = 0.0f; uv[0][1] = 1.0f;
        uv[1][0] = 0.0f; uv[1][1] = 0.0f;
        uv[2][0] = 1.0f; uv[2][1] = 0.0f;
        uv[3][0] = 1.0f; uv[3][1] = 1.0f;
    }
};


class BatchRenderer
{
public:

    BatchRenderer();
    static BatchRenderer& GetInstance();

    void Init();
    void BeginBatch();
    void Flush();
    void DrawQuad(float x, float y, float w, float h, GLuint textureID, TextureCoordinate tc = TextureCoordinate());
    void DrawQuad(float x, float y, float w, float h, GLuint textureID,
                                 TextureCoordinate tc, float r, float g, float b, float a);

    void LoadFont(const std::string& name, AAssetManager* mgr, const std::string& assetPath);
    void RenderText(const std::string& text, float x, float y, float fontSize,
        FontAsset& font,
        float cr = 1.f, float cg = 1.f, float cb = 1.f, float ca = 1.f);


private:
    GLuint VAO, VBO;
    GLuint m_pinkTexture;
    std::vector<Vertex> m_vertices;
    int m_max_vertices;
    GLuint mProgram;
    std::vector<GLuint> m_texture_slots; // Current textures in this batch
    const int m_max_texture_slots = 8;
};

#define DRAW_QUAD_FULL(x,y,w,h,ID) BatchRenderer::GetInstance().DrawQuad(x,y,w,h,ID)
#define DRAW_QUAD(x,y,w,h,ID,tc) BatchRenderer::GetInstance().DrawQuad(x,y,w,h,ID,tc)
#endif
