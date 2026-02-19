#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <GLES3/gl31.h>
#include "AssetManager.h"
#include <android/log.h>
#include <string>
#include <map>

#define LOG_TAG "ShaderManager"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

class ShaderManager
{
public:
    static ShaderManager& GetInstance(); // Singleton
    std::string LoadShaderSource(const char* path);
    GLuint CreateProgram(const char* shaderName);
    GLuint GetShader(std::string shaderName);

private:
    std::map<std::string, GLuint> m_shaderMap;
};


#endif
