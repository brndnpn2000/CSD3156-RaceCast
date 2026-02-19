//
// Created by brand on 28/1/2026.
//
extern float g_ScreenWidth;
extern float g_ScreenHeight;
#include "BatchRenderer.h"

BatchRenderer::BatchRenderer()
        : VAO(0), VBO(0), m_pinkTexture(0), mProgram(0),
          m_max_vertices(MAX_TRIANGLE * 3 * 2) // 100 quads = 600 vertices
{
}

BatchRenderer& BatchRenderer::GetInstance()
{
    static BatchRenderer instance;
    return instance;
}


void BatchRenderer::Init()
{
// 1. Setup Shader
    mProgram = ShaderManager::GetInstance().CreateProgram("BatchRenderer");
    LOGI("[BatchRenderer] Initialised(?) Shader BatchRenderer");
    // 2. Create Fallback Pink Texture (1x1 Magenta pixel)
    uint32_t pink = 0xFFFF00FF; // ABGR format
    glGenTextures(1, &m_pinkTexture);
    glBindTexture(GL_TEXTURE_2D, m_pinkTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &pink);

    // Set texture parameters so it doesn't look blurry
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // 3. Setup Buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Allocate memory for the maximum number of vertices using GL_DYNAMIC_DRAW
    glBufferData(GL_ARRAY_BUFFER, m_max_vertices * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

    // Attribute 0: Position (3 floats)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

    // Attribute 1: Color (4 floats)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

    // Attribute 2: Texture Coordinates (2 floats)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    // Attribute 3: Texture Index (1 float)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texIndex));

    glBindVertexArray(0); // Unbind

    glUseProgram(mProgram);
    GLint samplers[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    auto loc = glGetUniformLocation(mProgram, "uTextures");
    glUniform1iv(loc, 8, samplers); // Maps shader array to GL_TEXTURE0...GL_TEXTURE7

    m_texture_slots.clear();
    m_texture_slots.push_back(m_pinkTexture); // Slot 0 is now valid

    glEnable(GL_BLEND);
    // Standard transparency formula:
    // Result = (NewColor * NewAlpha) + (OldColor * (1 - NewAlpha))
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    LOGI("[BatchRenderer] Initialised BatchRenderer Instance");
}

void BatchRenderer::BeginBatch()
{
    m_vertices.clear(); // Clear the CPU-side vector for a new frame
}

void BatchRenderer::Flush() {
    if (m_vertices.empty()) return;

    glUseProgram(mProgram);

    // 1. Bind all textures to their respective slots
    for (size_t i = 0; i < m_texture_slots.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_texture_slots[i]);
    }

    // 2. Bind VAO and VBO once
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // 3. Upload vertex data once
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertices.size() * sizeof(Vertex), m_vertices.data());

    // 4. Draw!
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)m_vertices.size());

    // 5. Clean up
    glBindVertexArray(0);
    m_vertices.clear();
    m_texture_slots.clear();
    m_texture_slots.push_back(m_pinkTexture);
}

void BatchRenderer::DrawQuad(float x, float y, float w, float h, GLuint textureID, TextureCoordinate tc) {

    if (m_vertices.size() + 6 >= m_max_vertices) {
        Flush();
    }

    float halfW = w * 0.5f;
    float halfH = h * 0.5f;

    float left   = x - halfW;
    float right  = x + halfW;
    float bottom = y - halfH;
    float top    = y + halfH;

    // 2. Find or add the texture to a slot
    float texIndex = 0.0f;
    if (textureID != 0) {
        bool found = false;
        for (size_t i = 1; i < m_texture_slots.size(); i++) {
            if (m_texture_slots[i] == textureID) {
                texIndex = (float)i;
                found = true;
                break;
            }
        }

        if (!found) {
            if (m_texture_slots.size() >= m_max_texture_slots) {
                Flush();
            }
            texIndex = (float)m_texture_slots.size();
            m_texture_slots.push_back(textureID);
        }
    }

    // 3. Define the 4 vertices using the TextureCoordinate struct
    Vertex v[4];

    // Top-Left
    v[0].position[0] = left;  v[0].position[1] = top;    v[0].position[2] = 0.0f;
    v[0].texCoords[0] = tc.uv[0][0]; v[0].texCoords[1] = tc.uv[0][1];

    // Bottom-Left
    v[1].position[0] = left;  v[1].position[1] = bottom; v[1].position[2] = 0.0f;
    v[1].texCoords[0] = tc.uv[1][0]; v[1].texCoords[1] = tc.uv[1][1];

    // Bottom-Right
    v[2].position[0] = right; v[2].position[1] = bottom; v[2].position[2] = 0.0f;
    v[2].texCoords[0] = tc.uv[2][0]; v[2].texCoords[1] = tc.uv[2][1];

    // Top-Right
    v[3].position[0] = right; v[3].position[1] = top;    v[3].position[2] = 0.0f;
    v[3].texCoords[0] = tc.uv[3][0]; v[3].texCoords[1] = tc.uv[3][1];

    // Common data
    for (int i = 0; i < 4; i++) {
        v[i].color[0] = 1.0f; v[i].color[1] = 1.0f;
        v[i].color[2] = 1.0f; v[i].color[3] = 1.0f;
        v[i].texIndex = texIndex;
    }

    // 4. Push as two triangles
    m_vertices.push_back(v[0]); m_vertices.push_back(v[1]); m_vertices.push_back(v[2]);
    m_vertices.push_back(v[0]); m_vertices.push_back(v[2]); m_vertices.push_back(v[3]);
}

void BatchRenderer::DrawQuad(float x, float y, float w, float h, GLuint textureID,
                             TextureCoordinate tc, float r, float g, float b, float a) {

    if (m_vertices.size() + 6 >= m_max_vertices) {
        Flush();
    }

    float halfW = w * 0.5f;
    float halfH = h * 0.5f;

    float left   = x - halfW;
    float right  = x + halfW;
    float bottom = y - halfH;
    float top    = y + halfH;

    // 2. Texture Slot Logic (Unchanged)
    float texIndex = 0.0f;
    if (textureID != 0) {
        bool found = false;
        for (size_t i = 1; i < m_texture_slots.size(); i++) {
            if (m_texture_slots[i] == textureID) {
                texIndex = (float)i;
                found = true;
                break;
            }
        }

        if (!found) {
            if (m_texture_slots.size() >= m_max_texture_slots) {
                Flush();
            }
            texIndex = (float)m_texture_slots.size();
            m_texture_slots.push_back(textureID);
        }
    }

    // 3. Define the 4 vertices
    Vertex v[4];

    // Top-Left
    v[0].position[0] = left;  v[0].position[1] = top;    v[0].position[2] = 0.0f;
    v[0].texCoords[0] = tc.uv[0][0]; v[0].texCoords[1] = tc.uv[0][1];

    // Bottom-Left
    v[1].position[0] = left;  v[1].position[1] = bottom; v[1].position[2] = 0.0f;
    v[1].texCoords[0] = tc.uv[1][0]; v[1].texCoords[1] = tc.uv[1][1];

    // Bottom-Right
    v[2].position[0] = right; v[2].position[1] = bottom; v[2].position[2] = 0.0f;
    v[2].texCoords[0] = tc.uv[2][0]; v[2].texCoords[1] = tc.uv[2][1];

    // Top-Right
    v[3].position[0] = right; v[3].position[1] = top;    v[3].position[2] = 0.0f;
    v[3].texCoords[0] = tc.uv[3][0]; v[3].texCoords[1] = tc.uv[3][1];

    // --- APPLY THE TINT HERE ---
    for (int i = 0; i < 4; i++) {
        v[i].color[0] = r;
        v[i].color[1] = g;
        v[i].color[2] = b;
        v[i].color[3] = a;
        v[i].texIndex = texIndex;
    }

    // 4. Push as two triangles
    m_vertices.push_back(v[0]); m_vertices.push_back(v[1]); m_vertices.push_back(v[2]);
    m_vertices.push_back(v[0]); m_vertices.push_back(v[2]); m_vertices.push_back(v[3]);
}