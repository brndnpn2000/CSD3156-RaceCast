package com.example.racecast

import android.content.Context
import android.opengl.GLSurfaceView
import android.view.MotionEvent

class MyGLSurfaceView(context: Context) : GLSurfaceView(context) {

    // Declare the native function that your InputManager uses
    private external fun nativeOnTouch(action: Int, pointerId: Int, x: Float, y: Float)

    override fun onTouchEvent(event: MotionEvent): Boolean {
        val action = event.actionMasked
        val index = event.actionIndex
        val pointerId = event.getPointerId(index)

        // Use raw coordinates or standard ones depending on your conversion math
        val x = event.getX(index)
        val y = event.getY(index)

        // Handle the MOVE action differently to ensure all fingers update
        if (action == MotionEvent.ACTION_MOVE) {
            for (i in 0 until event.pointerCount) {
                nativeOnTouch(action, event.getPointerId(i), event.getX(i), event.getY(i))
            }
        } else {
            nativeOnTouch(action, pointerId, x, y)
        }

        return true
    }
}