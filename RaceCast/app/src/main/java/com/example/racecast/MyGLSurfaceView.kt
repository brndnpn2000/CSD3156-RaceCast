package com.example.racecast

import android.content.Context
import android.opengl.GLSurfaceView
import android.view.MotionEvent

class MyGLSurfaceView(context: Context) : GLSurfaceView(context) {

    override fun onTouchEvent(event: MotionEvent): Boolean {
        val action = event.actionMasked
        val index = event.actionIndex
        val pointerId = event.getPointerId(index)

        // Use raw coordinates or standard ones depending on your conversion math
        val touchX = event.getX(index)
        val touchY = event.getY(index)

        // Handle the MOVE action differently to ensure all fingers update
        if (action == MotionEvent.ACTION_MOVE) {
            for (i in 0 until event.pointerCount) {
                nativeOnTouch(
                    action = action,
                    pointerId = event.getPointerId(i),
                    touchX = event.getX(i),
                    touchY = event.getY(i)
                )
            }
        } else {
            nativeOnTouch(action, pointerId, touchX, touchY)
        }

        return true
    }

    // External functions are typically placed at the bottom of the class
    private external fun nativeOnTouch(action: Int, pointerId: Int, touchX: Float, touchY: Float)
}