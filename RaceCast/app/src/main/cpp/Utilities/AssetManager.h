#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <GLES3/gl31.h>
#include "stb_image.h"
#include <string>
#include <map>
#include "FontAsset.h"
#include "Logger.h"

#include <filesystem>
class AssetManager
{
public:
    AssetManager() : m_AssetManager(nullptr) {}

    static AssetManager& GetInstance();

    AAssetManager** GetContext();

    void LoadTexture(const char* path);
    GLuint GetTexture(const char* path);
    void CleanAll();
    void Reset();

    //fonts
    void LoadFont(const std::string& name, const std::string& path);
    FontAsset* GetFont(const std::string& name);

private:
    AAssetManager* m_AssetManager;
    std::map<std::string, GLuint> m_texture_map;


    //fonts
    std::map<std::string, std::shared_ptr<FontAsset>> m_font_map;   // ADD

};

#define NO_TEXTURE 0

#endif
