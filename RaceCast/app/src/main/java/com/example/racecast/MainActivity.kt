package com.example.racecast

import android.os.Bundle
import android.content.res.AssetManager
import android.opengl.GLSurfaceView
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.WindowInsetsCompat
import androidx.core.view.WindowInsetsControllerCompat
import android.os.Build
import android.view.WindowManager

import android.content.Context
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager

// --- FIX 1: Add the Interface here ---
class MainActivity : AppCompatActivity(), SensorEventListener {

    private external fun nativeInitAssetManager(assetManager: AssetManager)
    private lateinit var gLView: MyGLSurfaceView

    private lateinit var sensorManager: SensorManager
    private var accelerometer: Sensor? = null

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

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        sensorManager = getSystemService(Context.SENSOR_SERVICE) as SensorManager
        accelerometer = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER)

        hideSystemUI()

        nativeInitAssetManager(assets)

        gLView = MyGLSurfaceView(this).apply {
            setEGLContextClientVersion(3)
            setRenderer(MyRenderer())
        }

        setContentView(gLView)
    }

    // --- FIX 2: Implement the required methods for the Sensor ---
    override fun onSensorChanged(event: SensorEvent?) {
        event?.let {
            if (it.sensor.type == Sensor.TYPE_ACCELEROMETER) {
                // values[1] is Y-axis (Tilt for steering in landscape)
                setNativeTilt(it.values[1])
            }
        }
    }

    override fun onAccuracyChanged(sensor: Sensor?, accuracy: Int) {
        // Leave empty
    }

    override fun onPause() {
        super.onPause() // Only call this once
        gLView.onPause()
        sensorManager.unregisterListener(this)
    }

    override fun onResume() {
        super.onResume()
        accelerometer?.let {
            sensorManager.registerListener(this, it, SensorManager.SENSOR_DELAY_GAME)
        }
        gLView.onResume()
    }

    external fun setNativeTilt(tilt: Float)

    companion object {
        init {
            // Ensure this matches your CMake project name!
            System.loadLibrary("racecast")
        }
    }
}