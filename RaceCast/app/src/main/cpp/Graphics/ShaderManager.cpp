#include "ShaderManager.h"
extern AAssetManager* gAssetManager;

ShaderManager& ShaderManager::GetInstance()
{
    static ShaderManager instance;
    return instance;
}

std::string ShaderManager::LoadShaderSource(const char* path) {
    AAssetManager ** AssetMgr = AssetManager::GetInstance().GetContext();

    if (!*AssetMgr) {
        LOGE("AssetManager not initialized!");
        return "";
    }

    AAsset* asset = AAssetManager_open(*AssetMgr, path, AASSET_MODE_BUFFER);
    if (!asset) {
        LOGE("Could not open asset: %s", path);
        return "";
    }

    off_t length = AAsset_getLength(asset);
    std::vector<char> buffer(length + 1);
    AAsset_read(asset, buffer.data(), length);
    buffer[length] = '\0';

    AAsset_close(asset);
    return std::string(buffer.data());
}

GLuint ShaderManager::CreateProgram(const char* shaderName) {
    // 1. Check if the program already exists in the map
    if (m_shaderMap.count(shaderName)) {
        return m_shaderMap[shaderName];
    }

    // 2. Construct file paths (Assuming shaders are in "shaders/" folder)
    std::string vertPath = "shaders/" + std::string(shaderName) + ".vert";
    std::string fragPath = "shaders/" + std::string(shaderName) + ".frag";

    std::string vSrc = LoadShaderSource(vertPath.c_str());
    std::string fSrc = LoadShaderSource(fragPath.c_str());

    const char* vCode = vSrc.c_str();
    const char* fCode = fSrc.c_str();

    // Lambda for compiling stages
    auto compileShader = [](GLenum type, const char* source) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            LOGE("Shader Compilation Error: %s", infoLog);
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
    }

    // Clean up temporary shader objects
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    // Store in map and return
    m_shaderMap[shaderName] = program;
    LOGE("[SHADER] Loaded: %s", shaderName);
    return program;
}

GLuint ShaderManager::GetShader(std::string shaderName) {
// Look for the shader in the map
    auto it = m_shaderMap.find(shaderName);

    if (it != m_shaderMap.end()) {
        // Return the GLuint (program ID) stored in the map
        return it->second;
    }

    // Shader not found; return 0 (standard OpenGL error handle)
    __android_log_print(ANDROID_LOG_WARN, "ShaderManager", "Shader '%s' not found in map!", shaderName.c_str());
    return 0;
}
