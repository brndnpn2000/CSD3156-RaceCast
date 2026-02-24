package com.example.racecast

import android.opengl.GLSurfaceView
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class MyRenderer : GLSurfaceView.Renderer {

    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
        nativeInit()
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        nativeChanged(width, height)
    }

    override fun onDrawFrame(gl: GL10?) {
        nativeRender()
    }

    // JNI Methods
    private external fun nativeInit()
    private external fun nativeRender()
    private external fun nativeChanged(width: Int, height: Int)
}