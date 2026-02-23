#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <GLES3/gl31.h>
#include "AssetManager.h"
#include <android/log.h>
#include <string>
#include <map>
#include "Logger.h"

class ShaderManager
{
public:
    void Reset();

    static ShaderManager& GetInstance(); // Singleton
    std::string LoadShaderSource(const char* path);
    GLuint CreateProgram(const char* shaderName);
    GLuint GetShader(std::string shaderName);

private:
    std::map<std::string, GLuint> m_shaderMap;
};


#endif
