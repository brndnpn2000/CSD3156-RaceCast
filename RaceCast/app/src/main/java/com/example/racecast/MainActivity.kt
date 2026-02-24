package com.example.racecast

import android.content.Context
import android.content.res.AssetManager
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager
import android.opengl.GLSurfaceView
import android.os.Build
import android.os.Bundle
import android.view.Surface
import android.view.WindowManager
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.WindowInsetsCompat
import androidx.core.view.WindowInsetsControllerCompat

class MainActivity : AppCompatActivity(), SensorEventListener {

    private lateinit var glView: MyGLSurfaceView
    private lateinit var sensorManager: SensorManager
    private var accelerometer: Sensor? = null

    private val prefs by lazy { getSharedPreferences("racecast_prefs", MODE_PRIVATE) }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        sensorManager = getSystemService(Context.SENSOR_SERVICE) as SensorManager
        accelerometer = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER)

        hideSystemUI()

        nativeInitAssetManager(assets)
        loadLeaderboards()

        glView = MyGLSurfaceView(this).apply {
            setEGLContextClientVersion(3)
            setRenderer(MyRenderer())
        }

        setContentView(glView)
    }

    override fun onResume() {
        super.onResume()
        nativeOnResume()
        accelerometer?.let {
            sensorManager.registerListener(this, it, SensorManager.SENSOR_DELAY_GAME)
        }
        glView.onResume()
    }

    override fun onPause() {
        saveLeaderboards()
        nativeOnPause()
        glView.onPause()
        sensorManager.unregisterListener(this)
        super.onPause()
    }

    override fun onSensorChanged(event: SensorEvent) {
        if (event.sensor.type == Sensor.TYPE_ACCELEROMETER) {
            val display = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                display
            } else {
                @Suppress("DEPRECATION")
                windowManager.defaultDisplay
            }

            val rotation = display?.rotation ?: Surface.ROTATION_0
            var tilt = 0f

            when (rotation) {
                Surface.ROTATION_0 -> tilt = -event.values[0]
                Surface.ROTATION_90 -> tilt = event.values[1]
                Surface.ROTATION_180 -> tilt = event.values[0]
                Surface.ROTATION_270 -> tilt = -event.values[1]
            }

            setNativeTilt(tilt)
        }
    }

    override fun onAccuracyChanged(sensor: Sensor?, accuracy: Int) {
        // Required by SensorEventListener
    }

    private fun loadLeaderboards() {
        for (map in 1..3) {
            val key = "lb_map_$map"
            if (prefs.contains(key)) {
                val json = prefs.getString(key, "[]") ?: "[]"
                nativeSetLeaderboard(map, json)
            }
        }
    }

    private fun saveLeaderboards() {
        val editor = prefs.edit()
        for (map in 1..3) {
            val json = nativeGetLeaderboard(map)
            editor.putString("lb_map_$map", json)
        }
        editor.apply()
    }

    private fun hideSystemUI() {
        val windowInsetsController = WindowInsetsControllerCompat(window, window.decorView)
        windowInsetsController.hide(WindowInsetsCompat.Type.systemBars())
        windowInsetsController.systemBarsBehavior =
            WindowInsetsControllerCompat.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            window.attributes.layoutInDisplayCutoutMode =
                WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES
        }
    }

    // JNI Methods
    external fun nativeSetLeaderboard(mapIndex: Int, json: String)
    external fun nativeGetLeaderboard(mapIndex: Int): String
    external fun setNativeTilt(tilt: Float)
    external fun nativeOnPause()
    external fun nativeOnResume()
    private external fun nativeInitAssetManager(assetManager: AssetManager)

    companion object {
        init {
            System.loadLibrary("racecast")
        }
    }
}