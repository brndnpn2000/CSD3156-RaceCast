package com.example.racecast

import android.opengl.GLSurfaceView
import android.view.MotionEvent
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10


class MyRenderer : GLSurfaceView.Renderer {

    // You must declare EVERY native function you intend to call
    external fun nativeInit()
    external fun nativeRender()
    external fun nativeChanged(width: Int, height: Int) // Add this line if missing

    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
        nativeInit()
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        // Now this call will work because it's declared above
        nativeChanged(width, height)
    }

    override fun onDrawFrame(gl: GL10?) {
        nativeRender()
    }



}

