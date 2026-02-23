#include "ShaderManager.h"
extern AAssetManager* gAssetManager;

ShaderManager& ShaderManager::GetInstance()
{
    static ShaderManager instance;
    return instance;
}

std::string ShaderManager::LoadShaderSource(const char* path) {
    AAssetManager** AssetMgr = AssetManager::GetInstance().GetContext();

    if (!AssetMgr || !*AssetMgr) {
        LOGE("AssetManager not initialized!");
        return "";
    }

    AAsset* asset = AAssetManager_open(*AssetMgr, path, AASSET_MODE_BUFFER);
    if (!asset) {
        LOGE("Could not open asset: %s", path);
        return "";
    }

    off_t length = AAsset_getLength(asset);
    // Use a vector to safely manage memory
    std::vector<char> buffer(length);
    AAsset_read(asset, buffer.data(), length);
    AAsset_close(asset);

    // Construct string directly from buffer data and length
    return std::string(buffer.data(), length);
}

GLuint ShaderManager::CreateProgram(const char* shaderName) {
    // 1. Check if the program already exists in the map
    if (m_shaderMap.count(shaderName)) {
        return m_shaderMap[shaderName];
    }

    // 2. Load Source
    std::string vertPath = "shaders/" + std::string(shaderName) + ".vert";
    std::string fragPath = "shaders/" + std::string(shaderName) + ".frag";

    std::string vSrc = LoadShaderSource(vertPath.c_str());
    std::string fSrc = LoadShaderSource(fragPath.c_str());

    if (vSrc.empty() || fSrc.empty()) {
        LOGE("Failed to load shader source for: %s", shaderName);
        return 0;
    }

    const char* vCode = vSrc.c_str();
    const char* fCode = fSrc.c_str();

    auto compileShader = [](GLenum type, const char* source) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            LOGE("Shader Compilation Error (%d): %s", type, infoLog);
        }
        return shader;
    };

    GLuint vertex = compileShader(GL_VERTEX_SHADER, vCode);
    GLuint fragment = compileShader(GL_FRAGMENT_SHADER, fCode);

    // 3. Link Program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        LOGE("Program Linking Error: %s", infoLog);
        glDeleteProgram(program); // Clean up failed program
        return 0;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    m_shaderMap[shaderName] = program;
    LOGI("[SHADER] Successfully Created/Recovered: %s (ID: %d)", shaderName, program);
    return program;
}

GLuint ShaderManager::GetShader(std::string shaderName) {
    auto it = m_shaderMap.find(shaderName);
    if (it != m_shaderMap.end()) {
        return it->second;
    }
    LOGE("Shader '%s' not found in map!", shaderName.c_str());
    return 0;
}

void ShaderManager::Reset() {
    // IMPORTANT: Clear IDs without glDeleteProgram because the context
    // is already destroyed. Deleting invalid IDs can cause crashes.
    m_shaderMap.clear();
    LOGI("[SHADER_MANAGER] Internal map cleared for Context Recovery.");
}