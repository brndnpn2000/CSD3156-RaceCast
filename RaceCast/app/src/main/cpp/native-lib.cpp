#include <jni.h>
#include <GLES3/gl31.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <vector>
#include <string>

#include "AssetManager.h"
#include "GameStateManager.h"
#include "MenuState.h"
#include "ShaderManager.h"
#include "AudioManager.h"
#include "InputManager.h"
#include "Timer.h"
#include "Globals.h"
#include "HighScore.h"

#define LOG_TAG "RaceCast-Native"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

float g_ScreenWidth = 1.0f;
float g_ScreenHeight = 1.0f;
float g_DeviceTilt = 0.0f;
extern "C" {

JNIEXPORT void JNICALL
Java_com_example_racecast_MainActivity_nativeOnPause(JNIEnv* env, jobject thiz) {
    LOGI("[LIFECYCLE] nativeOnPause called - Pausing Audio");
    // Call the AudioManager function we created earlier to silence the engine
    AUDIO.PauseAll();
}

JNIEXPORT void JNICALL
Java_com_example_racecast_MainActivity_nativeOnResume(JNIEnv* env, jobject thiz) {
    LOGI("[LIFECYCLE] nativeOnResume called - Resuming Audio");
    // Resume the BGM or specific tracks
    AUDIO.ResumeAll();
}

    JNIEXPORT void JNICALL
    Java_com_example_racecast_MainActivity_nativeInitAssetManager(JNIEnv* env, jobject thiz, jobject assetManager)
    {
        AAssetManager** ptr = AssetManager::GetInstance().GetContext();
        *ptr = AAssetManager_fromJava(env, assetManager);
        if (ptr)
            LOGI("[ASSET_MANAGER] Initialized");
        else
            LOGE("[ASSET_MANAGER] Failed to Initialize");

        AUDIO.Init();
    }

JNIEXPORT void JNICALL
Java_com_example_racecast_MyRenderer_nativeInit(JNIEnv *env, jobject thiz)
{
    LOGI("[LIFECYCLE] nativeInit: Restoring OpenGL Context");
    ShaderManager::GetInstance().Reset();
    AssetManager::GetInstance().Reset();
    // 1. Re-initialize the BatchRenderer (Generates new VAO/VBO handles)
    BatchRenderer::GetInstance().Init();

    // 3. Asset Recovery: Re-upload textures for the current state
    if (GSM.GetCurrentState() != nullptr) {
        LOGI("[LIFECYCLE] Reloading assets for active state");
        // Re-run Init on the current state to re-upload its textures/buffers
        GSM.GetCurrentState()->Init();
    } else {
        // First time opening the app
        GSM.ChangeState(new MenuState());
    }
}

    JNIEXPORT void JNICALL
    Java_com_example_racecast_MyRenderer_nativeChanged(JNIEnv *env, jobject thiz, jint width, jint height)
    {
        glViewport(0, 0, width, height);
        g_ScreenWidth = (float)width;
        g_ScreenHeight = (float)height;
        Globals::screen_size.first = (float)width;
        Globals::screen_size.second = (float)height;

    }

    // ENTRY POINT
    JNIEXPORT void JNICALL
    Java_com_example_racecast_MyRenderer_nativeRender(JNIEnv *env, jobject thiz)
    {
        TIMER.Update();
        float deltaTime = TIMER.GetDeltaTime();
        InputManager::GetInstance().Update();
        GSM.Update(deltaTime);
        GSM.Render();
    }

JNIEXPORT void JNICALL
Java_com_example_racecast_MyGLSurfaceView_nativeOnTouch(JNIEnv *env, jobject thiz,
                                                        jint action, jint pointerId,
                                                        jfloat x, jfloat y) {

    // 1. Convert pixels to NDC (-1 to 1)
    // Use the screen dimensions we stored in nativeResize
    float ndcX = (x / g_ScreenWidth) * 2.0f - 1.0f;
    float ndcY = 1.0f - (y / g_ScreenHeight) * 2.0f;

    // 2. Pass to your InputManager
    InputManager::GetInstance().HandleTouch(action, pointerId, ndcX, ndcY);
}
    JNIEXPORT void JNICALL
    Java_com_example_racecast_MainActivity_setNativeTilt(JNIEnv* env, jobject thiz, jfloat tilt) {
        // Update your global variable or player steering here
        g_DeviceTilt = tilt;
    }

    JNIEXPORT void JNICALL
    Java_com_example_racecast_MainActivity_nativeSetLeaderboard(
            JNIEnv* env, jobject /*thiz*/, jint mapIndex, jstring jsonStr
    ) {
        const char* cstr = env->GetStringUTFChars(jsonStr, nullptr);
        std::string s(cstr ? cstr : "");
        env->ReleaseStringUTFChars(jsonStr, cstr);

        auto vec = HighScore::DeserializeScoreList(s);
        auto& list = HighScore::getMapScoreList((int)mapIndex);
        list = std::move(vec);
    }
    JNIEXPORT jstring JNICALL
    Java_com_example_racecast_MainActivity_nativeGetLeaderboard(
            JNIEnv* env, jobject /*thiz*/, jint mapIndex
    ) {
        auto& list = HighScore::getMapScoreList((int)mapIndex);
        std::string s = HighScore::SerializeScoreList(list);
        return env->NewStringUTF(s.c_str());
    }

}