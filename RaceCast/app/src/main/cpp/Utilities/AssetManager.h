#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <GLES3/gl31.h>
#include "stb_image.h"
#include <string>
#include <map>

#include <android/log.h>
#define LOG_TAG "RaceCast-Native"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
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

private:
    AAssetManager* m_AssetManager;
    std::map<std::string, GLuint> m_texture_map;

};

#define NO_TEXTURE 0

#endif
