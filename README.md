# RaceCast

**RaceCast** is a high-performance, 100% native 3D racing game for Android, built from the ground up using a custom **Raycasting Engine** in C++. This project was developed as part of the **CSD3156** curriculum at **DigiPen Institute of Technology**.

The engine demonstrates the power of the Android NDK by handling graphics, audio, and physics entirely in native code, bypassing the JVM for maximum performance.



---

## Technical Highlights

* **Custom 3D Raycasting:** Implemented a DDA (Digital Differential Analyzer) algorithm to project a 2D grid into a 3D environment, optimized for mobile ARM architectures.
* **100% Native Architecture:** All core systems (Rendering, Audio, GSM) are written in C++, utilizing the **Android NDK** to minimize latency.
* **Hardware Sensor Integration:** Real-time steering via device **Accelerometer** and touch inputs bridged from Java/Kotlin to C++ via a custom **JNI** layer.
* **Native Audio Engine:** Built using **OpenSL ES**, featuring a custom `AudioManager` that supports PCM buffer queues and hardware-accelerated MP3 decoding.
* **Modern Graphics Pipeline:** Powered by **OpenGL ES 3.1** with custom GLSL shaders and an efficient Batch Renderer for UI and 2D elements.

---

## Architecture & Design Patterns

The project follows a decoupled architecture, separating the Android platform lifecycle from the core game simulation.

### **1. The JNI Bridge**
The Android Activity acts as a host, passing hardware events (Touch, Tilt) and the `AAssetManager` to the native layer.
* **`native-lib.cpp`**: The primary entry point synchronizing the C++ Game State Manager (GSM) with the Android `onDrawFrame` lifecycle.

### **2. Core Engine Modules**
| Module | Description |
| :--- | :--- |
| **Graphics** | RayCaster for world rendering and BatchRenderer for optimized UI drawing. |
| **GSM** | A state-driven system (State Pattern) managing Menus, Selection, and Gameplay. |
| **Audio** | Native OpenSL ES implementation with event-driven callbacks for state management. |
| **Utilities** | Asset loading via `AAssetManager` and high-resolution Frame Timers. |


### **3. Persistent Highscore**
- Runtime (C++)
    - Leaderboards live in C++: HighScore::mapScores[0..2] (3 maps → 3 std::vector<float>)
    - When race ends (isEnded() turns true):
        - capture once: final_time = race_timer
        - save once: push into the correct map vector
- Persistence (Kotlin + SharedPreferences)
    - Kotlin stores each map’s leaderboard as a String in SharedPreferences:
        - keys: lb_map_0, lb_map_1, lb_map_2
        - value: serialized list like "[12.3, 13.9, 15.0]"
- Load (read from disk → C++)
    - MainActivity.onCreate():
        - Kotlin reads saved strings from SharedPreferences
        - Kotlin calls nativeSetLeaderboard(mapIndex, jsonString)
        - C++ deserializes and fills mapScores[mapIndex]
- Save (write to disk ← C++)
    - MainActivity.onPause():
     - Kotlin calls nativeGetLeaderboard(mapIndex)
     - C++ serializes mapScores[mapIndex] and returns string
     - Kotlin writes the string into SharedPreferences
---

## Controls

* **Steering**: Tilt the device left or right (Accelerometer input).
* **Navigation**: On-screen UI buttons calibrated using Normalized Device Coordinates (NDC) for cross-device compatibility.
* **Interaction**: Touch the screen to start the race or navigate menus.

---

## 🛠️ Build & Installation

1.  **Prerequisites**:
    * Android Studio Ladybug or later.
    * Android NDK (Version 27.0.12077973).
    * CMake 3.22.1+.
2.  **Clone the Repository**:
    ```bash
    git clone [https://github.com/brand/CSD3156-RaceCast.git](https://github.com/brand/CSD3156-RaceCast.git)
    ```
3.  **Build**: Open the project in Android Studio. Gradle will invoke CMake to compile the native library.

---

## 📝 Engineering Challenges & Solutions

* **Memory Safety:** Successfully debugged and resolved `SIGSEGV` (Segmentation Faults) in the native layer by implementing robust null-pointer checks and validating OpenSL ES hardware interface realization.
* **Resource Management:** Designed an `AssetManager` that loads textures and audio directly from the APK's assets folder using native pointers, avoiding memory duplication between the Java and C++ layers.
* **Mobile Optimization:** Utilized fixed-point math concepts and efficient DDA traversal to maintain a consistent 60 FPS on mobile devices.
