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
#include "Logger.h"

// Constants & Globals
float g_ScreenWidth = 1.0f;
float g_ScreenHeight = 1.0f;
float g_DeviceTilt = 0.0f;

extern "C" {

JNIEXPORT void JNICALL
Java_com_example_racecast_MainActivity_nativeOnPause(JNIEnv* env, jobject /* thiz */) {
    LOGI("[LIFECYCLE] nativeOnPause: Pausing Audio");
    AUDIO.PauseAll();
}

JNIEXPORT void JNICALL
Java_com_example_racecast_MainActivity_nativeOnResume(JNIEnv* env, jobject /* thiz */) {
    LOGI("[LIFECYCLE] nativeOnResume: Resuming Audio");
    AUDIO.ResumeAll();
}

JNIEXPORT void JNICALL
Java_com_example_racecast_MainActivity_nativeInitAssetManager(JNIEnv* env, jobject /* thiz */, jobject assetManager) {
    AAssetManager** ptr = AssetManager::GetInstance().GetContext();
    *ptr = AAssetManager_fromJava(env, assetManager);

    if (*ptr) {
        LOGI("[ASSET_MANAGER] Initialized successfully");
    } else {
        LOGE("[ASSET_MANAGER] Failed to initialize from Java");
    }

    AUDIO.Init();
}

JNIEXPORT void JNICALL
Java_com_example_racecast_MyRenderer_nativeInit(JNIEnv* env, jobject /* thiz */) {
    LOGI("[LIFECYCLE] nativeInit: Restoring OpenGL Context");

    ShaderManager::GetInstance().Reset();
    AssetManager::GetInstance().Reset();
    BatchRenderer::GetInstance().Init();

    if (GSM.GetCurrentState() != nullptr) {
        LOGI("[LIFECYCLE] Reloading assets for active state");
        GSM.GetCurrentState()->Init();
    } else {
        GSM.ChangeState(new MenuState());
    }
}

JNIEXPORT void JNICALL
Java_com_example_racecast_MyRenderer_nativeChanged(JNIEnv* env, jobject /* thiz */, jint width, jint height) {
    glViewport(0, 0, width, height);

    g_ScreenWidth = static_cast<float>(width);
    g_ScreenHeight = static_cast<float>(height);

    Globals::screen_size.first = g_ScreenWidth;
    Globals::screen_size.second = g_ScreenHeight;
}

JNIEXPORT void JNICALL
Java_com_example_racecast_MyRenderer_nativeRender(JNIEnv* env, jobject /* thiz */) {
    TIMER.Update();
    float deltaTime = TIMER.GetDeltaTime();

    InputManager::GetInstance().Update();
    GSM.Update(deltaTime);
    GSM.Render();
}

JNIEXPORT void JNICALL
Java_com_example_racecast_MyGLSurfaceView_nativeOnTouch(JNIEnv* env, jobject /* thiz */,
                                                        jint action, jint pointerId,
                                                        jfloat touchX, jfloat touchY) {
    // Convert screen space pixels to NDC (-1 to 1)
    float ndcX = (touchX / g_ScreenWidth) * 2.0f - 1.0f;
    float ndcY = 1.0f - (touchY / g_ScreenHeight) * 2.0f;

    InputManager::GetInstance().HandleTouch(action, pointerId, ndcX, ndcY);
}

JNIEXPORT void JNICALL
Java_com_example_racecast_MainActivity_setNativeTilt(JNIEnv* env, jobject /* thiz */, jfloat tilt) {
    g_DeviceTilt = tilt;
}

JNIEXPORT void JNICALL
Java_com_example_racecast_MainActivity_nativeSetLeaderboard(JNIEnv* env, jobject /* thiz */, jint mapIndex, jstring jsonStr) {
    const char* cstr = env->GetStringUTFChars(jsonStr, nullptr);
    std::string s(cstr ? cstr : "");
    env->ReleaseStringUTFChars(jsonStr, cstr);

    auto vec = HighScore::DeserializeScoreList(s);
    auto& list = HighScore::getMapScoreList(static_cast<int>(mapIndex));
    list = std::move(vec);
}

JNIEXPORT jstring JNICALL
Java_com_example_racecast_MainActivity_nativeGetLeaderboard(JNIEnv* env, jobject /* thiz */, jint mapIndex) {
    auto& list = HighScore::getMapScoreList(static_cast<int>(mapIndex));
    std::string s = HighScore::SerializeScoreList(list);
    return env->NewStringUTF(s.c_str());
}

} // extern "C"