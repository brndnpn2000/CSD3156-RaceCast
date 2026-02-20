//
// Created by brand on 20/2/2026.
//

#include "AudioManager.h"

AudioManager& AudioManager::GetInstance()
{
    static AudioManager instance;
    return instance;
}

void AudioManager::Init()
{
    SLresult result;

    // 1. Create Engine Object
    result = slCreateEngine(&m_EngineObject, 0, nullptr, 0, nullptr, nullptr);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("[AUDIO_MANAGER] Failed to Create Engine");
        return;
    }

    // 2. Realize the Engine
    result = (*m_EngineObject)->Realize(m_EngineObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("[AUDIO_MANAGER] Failed to Realize Engine");
        return;
    }

    // 3. Get Engine Interface
    result = (*m_EngineObject)->GetInterface(m_EngineObject, SL_IID_ENGINE, &m_EngineEngine);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("[AUDIO_MANAGER] Failed to get Engine Interface");
        return;
    }

    // 4. Create Output Mix
    result = (*m_EngineEngine)->CreateOutputMix(m_EngineEngine, &m_OutputMixObject, 0, nullptr, nullptr);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("[AUDIO_MANAGER] Failed to Create Output Mix");
        return;
    }

    // 5. Realize Output Mix
    result = (*m_OutputMixObject)->Realize(m_OutputMixObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("[AUDIO_MANAGER] Failed to Realize Output Mix");
        return;
    }

    LOGI("[AUDIO_MANAGER] Successfully Initialized Engine and Output Mix");
}

void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
    AudioBuffer* buffer = (AudioBuffer*)context;
    (*bq)->Enqueue(bq, buffer->data.data(), buffer->data.size());
}

void AudioManager::LoadAudio(const char *path) {
    AAssetManager* mgr = *AssetManager::GetInstance().GetContext();
    AAsset* asset = AAssetManager_open(mgr, path, AASSET_MODE_UNKNOWN);
    if (!asset) return;

    // 1. Get the File Descriptor from the APK
    off_t start, length;
    int fd = AAsset_openFileDescriptor(asset, &start, &length);
    AAsset_close(asset);

    // 2. Setup Data Source for Compressed Files (MIME)
    SLDataLocator_AndroidFD loc_fd = { SL_DATALOCATOR_ANDROIDFD, fd, start, length };
    SLDataFormat_MIME format_mime = { SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED };
    SLDataSource audioSrc = { &loc_fd, &format_mime };

    // 3. Setup Sink (Output Mix)
    SLDataLocator_OutputMix loc_outmix = { SL_DATALOCATOR_OUTPUTMIX, m_OutputMixObject };
    SLDataSink audioSnk = { &loc_outmix, nullptr };

    // 4. Create Player (No Buffer Queue interface needed for FD)
    const SLInterfaceID ids[2] = { SL_IID_PLAY, SL_IID_VOLUME };
    const SLboolean req[2] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };

    AudioBuffer buffer;
    SLresult result = (*m_EngineEngine)->CreateAudioPlayer(m_EngineEngine, &buffer.playerObject, &audioSrc, &audioSnk, 2, ids, req);

    if (result == SL_RESULT_SUCCESS) {
        (*buffer.playerObject)->Realize(buffer.playerObject, SL_BOOLEAN_FALSE);
        (*buffer.playerObject)->GetInterface(buffer.playerObject, SL_IID_PLAY, &buffer.playerPlay);
        (*buffer.playerObject)->GetInterface(buffer.playerObject, SL_IID_VOLUME, &buffer.playerVolume);

        m_AudioMap[path] = std::move(buffer);
        LOGI("[AUDIO_MANAGER] Successfully Loaded Compressed Audio: %s", path);
    }
}

void AudioManager::PlayAudio(const char* name) {
    auto it = m_AudioMap.find(name);
    if (it == m_AudioMap.end() || it->second.playerPlay == nullptr) return;

    // 5. Ensure Play State is set to PLAYING
    SLresult res = (*it->second.playerPlay)->SetPlayState(it->second.playerPlay, SL_PLAYSTATE_PLAYING);
    if(res == SL_RESULT_SUCCESS) {
        LOGI("[AUDIO_MANAGER] Playing: %s", name);
    }
}

void AudioManager::UpdateAudioVolume(const char* name, float volume) {
    auto it = m_AudioMap.find(name);
    if (it == m_AudioMap.end() || it->second.playerVolume == nullptr) {
        return;
    }

    // Clamp volume between 0 and 1
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;

    // Convert linear volume (0->1) to millibels (0 -> -10000)
    // Formula: 2000 * log10(volume)
    SLmillibel level;
    if (volume <= 0.001f) {
        level = SL_MILLIBEL_MIN; // Silence
    } else {
        level = (SLmillibel)(2000 * log10(volume));
    }

    (*it->second.playerVolume)->SetVolumeLevel(it->second.playerVolume, level);
}

void AudioManager::Shutdown() {
    for (auto& pair : m_AudioMap) {
        if (pair.second.playerObject != nullptr) {
            // Stop playback before destroying
            (*pair.second.playerPlay)->SetPlayState(pair.second.playerPlay, SL_PLAYSTATE_STOPPED);

            // Destroy the player object
            (*pair.second.playerObject)->Destroy(pair.second.playerObject);
            pair.second.playerObject = nullptr;
            pair.second.playerPlay = nullptr;
            pair.second.playerVolume = nullptr;
        }
        pair.second.data.clear();
    }
    m_AudioMap.clear();

    if (m_OutputMixObject != nullptr) {
        (*m_OutputMixObject)->Destroy(m_OutputMixObject);
        m_OutputMixObject = nullptr;
    }

    if (m_EngineObject != nullptr) {
        (*m_EngineObject)->Destroy(m_EngineObject);
        m_EngineObject = nullptr;
        m_EngineEngine = nullptr;
    }
}


