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

void playEventCallback(SLPlayItf caller, void *pContext, SLuint32 event) {
    if (event & SL_PLAYEVENT_HEADATEND) {
        // Reset the state so PlayAudio() can trigger it again
        (*caller)->SetPlayState(caller, SL_PLAYSTATE_STOPPED);
        __android_log_print(ANDROID_LOG_INFO, "RaceCast-Audio", "Audio finished and reset to stopped state.");
    }
}

void AudioManager::LoadAudio(const char *path) {
    if (m_AudioMap.find(path) != m_AudioMap.end()) return;

    AAssetManager* mgr = *AssetManager::GetInstance().GetContext();
    AAsset* asset = AAssetManager_open(mgr, path, AASSET_MODE_UNKNOWN);
    if (!asset) return;

    off_t start, length;
    int fd = AAsset_openFileDescriptor(asset, &start, &length);
    AAsset_close(asset);

    SLDataLocator_AndroidFD loc_fd = { SL_DATALOCATOR_ANDROIDFD, fd, start, length };
    SLDataFormat_MIME format_mime = { SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED };
    SLDataSource audioSrc = { &loc_fd, &format_mime };
    SLDataLocator_OutputMix loc_outmix = { SL_DATALOCATOR_OUTPUTMIX, m_OutputMixObject };
    SLDataSink audioSnk = { &loc_outmix, nullptr };

    const SLInterfaceID ids[2] = { SL_IID_PLAY, SL_IID_VOLUME };
    const SLboolean req[2] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };

    AudioBuffer buffer;
    SLresult result = (*m_EngineEngine)->CreateAudioPlayer(m_EngineEngine, &buffer.playerObject, &audioSrc, &audioSnk, 2, ids, req);

    if (result == SL_RESULT_SUCCESS) {
        (*buffer.playerObject)->Realize(buffer.playerObject, SL_BOOLEAN_FALSE);
        (*buffer.playerObject)->GetInterface(buffer.playerObject, SL_IID_PLAY, &buffer.playerPlay);
        (*buffer.playerObject)->GetInterface(buffer.playerObject, SL_IID_VOLUME, &buffer.playerVolume);

        // --- NEW: Register Callback Logic ---
        // 1. Set the callback function
        (*buffer.playerPlay)->RegisterCallback(buffer.playerPlay, playEventCallback, nullptr);
        // 2. Tell OpenSL to notify us when the "Head" reaches the "End"
        (*buffer.playerPlay)->SetCallbackEventsMask(buffer.playerPlay, SL_PLAYEVENT_HEADATEND);

        m_AudioMap[path] = std::move(buffer);
        LOGI("[AUDIO_MANAGER] Successfully Loaded with Callback: %s", path);
    }
}

void AudioManager::PlayAudio(const char* name) {
    auto it = m_AudioMap.find(name);
    if (it == m_AudioMap.end() || it->second.playerPlay == nullptr) return;

    SLuint32 currentState;
    (*it->second.playerPlay)->GetPlayState(it->second.playerPlay, &currentState);

    // If it's already playing, do nothing.
    // If it's paused or stopped, resume from current head position.
    if (currentState != SL_PLAYSTATE_PLAYING) {
        (*it->second.playerPlay)->SetPlayState(it->second.playerPlay, SL_PLAYSTATE_PLAYING);
        LOGI("[AUDIO_MANAGER] Playing/Resuming: %s", name);
    }
}

void AudioManager::PlayLoopingAudio(const char* name) {
    auto it = m_AudioMap.find(name);
    if (it == m_AudioMap.end() || it->second.playerPlay == nullptr) return;

    // 1. Get the Seek Interface if not already stored
    if (it->second.playerSeek == nullptr) {
        (*it->second.playerObject)->GetInterface(it->second.playerObject, SL_IID_SEEK, &it->second.playerSeek);
    }

    // 2. Enable Looping at the hardware level
    if (it->second.playerSeek != nullptr) {
        (*it->second.playerSeek)->SetLoop(it->second.playerSeek, SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN);
    }

    // 3. Check current state to avoid restarting an already looping track
    SLuint32 currentState;
    (*it->second.playerPlay)->GetPlayState(it->second.playerPlay, &currentState);

    if (currentState != SL_PLAYSTATE_PLAYING) {
        (*it->second.playerPlay)->SetPlayState(it->second.playerPlay, SL_PLAYSTATE_PLAYING);
        LOGI("[AUDIO_MANAGER] BGM Loop Started: %s", name);
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


void AudioManager::StopAudio(const char* name) {
    auto it = m_AudioMap.find(name);
    if (it == m_AudioMap.end() || it->second.playerPlay == nullptr) return;

    // Pausing keeps the playback position (Head) where it is
    (*it->second.playerPlay)->SetPlayState(it->second.playerPlay, SL_PLAYSTATE_PAUSED);
    LOGI("[AUDIO_MANAGER] Paused: %s", name);
}

void AudioManager::ResetAudio(const char* name) {
    auto it = m_AudioMap.find(name);
    if (it == m_AudioMap.end() || it->second.playerPlay == nullptr) return;

    // 1. Get the Seek interface if we haven't already
    if (it->second.playerSeek == nullptr) {
        (*it->second.playerObject)->GetInterface(it->second.playerObject, SL_IID_SEEK, &it->second.playerSeek);
    }

    // 2. Seek to 0 milliseconds
    if (it->second.playerSeek != nullptr) {
        (*it->second.playerSeek)->SetPosition(it->second.playerSeek, 0, SL_SEEKMODE_FAST);
        LOGI("[AUDIO_MANAGER] Reset position to 0ms: %s", name);
    }
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


