package com.baidu.trafficplatedetect;

import org.opencv.core.Mat;
import org.opencv.core.MatOfRect;

public class TrafficPlateDetect 
{
	public TrafficPlateDetect(String cascadeName, String ocrfilenmae) 
	{
		mDetObj = nativeCreateObject(cascadeName, ocrfilenmae);
    }
	
    public void detect(Mat imageGray, MatOfRect plates, String strLisenceNum) 
    {
        nativeDetect(mDetObj, imageGray.getNativeObjAddr(), plates.getNativeObjAddr(), strLisenceNum);
    }
    
    public void release() 
    {
        nativeDestroyObject(mDetObj);
        mDetObj = 0;
    }
	
	private long mDetObj = 0;
	private long mRecObj = 0;
	
	private static native long nativeCreateObject(String cascadeName, String ocrfilenmae);
	private static native void nativeDestroyObject(long thiz);
    private static native void nativeDetect(long thiz, long inputImage, long faces, String strLisenceNum);
}
