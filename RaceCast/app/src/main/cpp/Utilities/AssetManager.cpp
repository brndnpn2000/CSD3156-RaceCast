#include "AssetManager.h"

AssetManager &AssetManager::GetInstance() {
    static AssetManager instance;
    return instance;
}

AAssetManager **AssetManager::GetContext() {
    return &m_AssetManager;
}

void AssetManager::CleanAll()
{
    for (auto& texture : m_texture_map)
    {
        glDeleteTextures(1, &texture.second);
    }

}

void AssetManager::LoadTexture(const char *path)
{
    stbi_set_flip_vertically_on_load(true);
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set wrapping/filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Load from AssetManager
    AAsset* asset = AAssetManager_open(*AssetManager::GetInstance().GetContext(), path, AASSET_MODE_BUFFER);
    if (!asset) {
        LOGE("[ASSET_MANAGER] Failed to load texture: %s", path);
    }

    off_t length = AAsset_getLength(asset);
    unsigned char* buffer = (unsigned char*)AAsset_getBuffer(asset);

    int width, height, channels;
    // Use stb_image to decode the buffer
    unsigned char* data = stbi_load_from_memory(buffer, length, &width, &height, &channels, 4);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
        LOGI("[ASSET_MANAGER] Loaded Texture : %s", path);
        m_texture_map[std::filesystem::path(path).filename().generic_string()] = textureID;
    }

    AAsset_close(asset);
}

GLuint AssetManager::GetTexture(const char* path)
{
    auto it = m_texture_map.find(path);
    if (it != m_texture_map.end())
    {
        return it->second;
    }
    return 0;
}

