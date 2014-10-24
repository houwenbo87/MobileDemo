package com.baidu.trafficplaterecon;

import org.opencv.core.Mat;

public class TrafficplateRecon {
    public TrafficplateRecon() {
        mNativeObj = nativeCreateObject();
    }

    public String detect(Mat imageGray) {
        return nativeDetect(mNativeObj, imageGray.getNativeObjAddr());
    }

    public void release() {
        nativeDestroyObject(mNativeObj);
        mNativeObj = 0;
    }

    private long mNativeObj = 0;

    private static native long nativeCreateObject();
    private static native void nativeDestroyObject(long thiz);
    private static native String nativeDetect(long thiz, long inputImage);
}
