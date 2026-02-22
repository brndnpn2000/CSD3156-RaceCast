# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

cmake_minimum_required(VERSION 3.22.1)
project("racecast")


add_subdirectory(freetype/freetype-2.10.0)


# *IMPORTANT* How Cmake find .cpp files
add_library(${CMAKE_PROJECT_NAME} SHARED
        native-lib.cpp
        GSM/GameState.cpp
        GSM/MenuState.cpp
        GSM/SelectionState.cpp
        GSM/PlayState.cpp
        GSM/Map1.cpp
        GSM/GameStateManager.cpp
        Graphics/RayCaster.cpp
        Graphics/BatchRenderer.cpp
        Graphics/ShaderManager.cpp
        Utilities/AssetManager.cpp
        Utilities/AudioManager.cpp
        Utilities/InputManager.cpp
        Utilities/UIHelper.cpp
        Utilities/Globals.cpp
        Utilities/Timer.cpp
        External/stb_impl.cpp
        Game/GameObject.cpp
        Game/Environment.cpp
        Game/Environment.tpp
        Game/CheckpointManager.cpp
        Game/HighScore.cpp
        Graphics/FontAsset.cpp

)

# *IMPORTANT* How CMake find .h files
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        .
        GSM
        Graphics
        Utilities
        External
        Game
        freetype/freetype-2.10.0/include

)

find_library(gles3-lib GLESv3)
find_library(log-lib log)
find_library(android-lib android)
find_library(sles-lib OpenSLES) # Add this line

target_link_libraries(${CMAKE_PROJECT_NAME}
        ${gles3-lib}
        ${log-lib}
        ${android-lib}
        ${sles-lib}           # Link the OpenSLES library variable [cite: 19, 23]
        atomic                # Good practice for C++17 atomics in NDK
        m                     # Links the math library
	freetype

)

# Specifies libraries CMake should link to your target library. You
# can link libraries from various origins, such as libraries defined in this
# build script, prebuilt third-party libraries, or Android system libraries.
target_link_libraries(${CMAKE_PROJECT_NAME}
        # List libraries link to the target library
        GLESv3
        android
	freetype
        log)


