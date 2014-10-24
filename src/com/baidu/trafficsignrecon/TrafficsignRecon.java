package com.baidu.trafficsignrecon;

import org.opencv.core.Mat;

public class TrafficsignRecon {
    public TrafficsignRecon() {
    	// 文件路径写死在函数中
        mNativeObj = nativeCreateObject();
    }

    public long detect(Mat imageGray) {
        return nativeDetect(mNativeObj, imageGray.getNativeObjAddr());
    }

    public void release() {
        nativeDestroyObject(mNativeObj);
        mNativeObj = 0;
    }

    private long mNativeObj = 0;

    private static native long nativeCreateObject();
    private static native void nativeDestroyObject(long thiz);
    private static native long nativeDetect(long thiz, long inputImage);
}
