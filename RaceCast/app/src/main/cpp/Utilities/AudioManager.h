#ifndef RACECAST_AUDIOMANAGER_H
#define RACECAST_AUDIOMANAGER_H

#include "AssetManager.h"
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <map>
#include <string>
#include <vector>

struct AudioBuffer {
    // Raw PCM data loaded from the asset
    std::vector<char> data;

    // OpenSL ES Player Objects and Interfaces
    SLObjectItf playerObject = nullptr;
    SLPlayItf playerPlay = nullptr;
    SLVolumeItf playerVolume = nullptr;
    SLSeekItf playerSeek = nullptr; // Added for looping
    SLAndroidSimpleBufferQueueItf bufferQueue = nullptr;

    // Default constructor for map compatibility
    AudioBuffer() = default;

    // Destructor to ensure cleanup if a buffer is removed
    void Cleanup() {
        if (playerObject != nullptr) {
            (*playerObject)->Destroy(playerObject);
            playerObject = nullptr;
            playerPlay = nullptr;
            playerVolume = nullptr;
            bufferQueue = nullptr;
        }
        data.clear();
    }
};

// Example AudioManager structure
class AudioManager
{
public:
    static AudioManager& GetInstance();

    void Init();
    // load audio from path, eg. "audio/bgm.mp3"
    void LoadAudio(const char* path);

    // play audio from where it left off
    void PlayAudio(const char* soundName);

    // play looping audio, eg. bgm
    void PlayLoopingAudio(const char* soundName);

    // pause audio
    void StopAudio(const char* name);

    // reset back to 0.00, but not working i think
    void ResetAudio(const char* name);

    void UpdateAudioVolume(const char* soundName, float volume);

    void Shutdown();
private:
    AudioManager() = default;

    // Engine and Output Mix objects
    SLObjectItf m_EngineObject = nullptr;
    SLEngineItf m_EngineEngine = nullptr;
    SLObjectItf m_OutputMixObject = nullptr;

    std::map<std::string, AudioBuffer> m_AudioMap;
};

#define AUDIO AudioManager::GetInstance()

#endif //RACECAST_AUDIOMANAGER_H
