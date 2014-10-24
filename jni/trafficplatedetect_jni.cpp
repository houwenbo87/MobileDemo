/*
 * trafficplatedetect_jni.cpp
 *
 *  Created on: 2014-7-30
 *      Author: houwenbo
 */

#include <trafficplatedetect_jni.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

using namespace std;
using namespace cv;

#include <stdlib.h>

#include <android/log.h>

#include "util.h"

inline void vector_Rect_to_Mat(vector<Rect>& v_rect, Mat& mat)
{
    mat = Mat(v_rect, true);
}


JNIEXPORT jlong JNICALL Java_com_baidu_trafficplatedetect_TrafficPlateDetect_nativeCreateObject
  (JNIEnv * jenv, jclass, jstring jFileName, jstring jocrfile)
{
    LOGD("Java_com_baidu_trafficplatedetect_TrafficPlateDetect_nativeCreateObject enter");
    const char* jnamestr = jenv->GetStringUTFChars(jFileName, NULL);
    string stdFileName(jnamestr);
    jlong result = 0;

    try
    {
        result = (jlong)new CascadeClassifier(stdFileName);
    }
    catch(cv::Exception& e)
    {
        LOGD("nativeCreateObject caught cv::Exception: %s", e.what());
        jclass je = jenv->FindClass("org/opencv/core/CvException");
        if(!je)
            je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, e.what());
    }
    catch (...)
    {
        LOGD("nativeCreateObject caught unknown exception");
        jclass je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, "Unknown exception in JNI code of DetectionBasedTracker.nativeCreateObject()");
        return 0;
    }

    LOGD("Java_com_baidu_trafficplatedetect_TrafficPlateDetect_nativeCreateObject exit");
    return result;
}


JNIEXPORT void JNICALL Java_com_baidu_trafficplatedetect_TrafficPlateDetect_nativeDetect
  (JNIEnv * jenv, jclass, jlong thiz, jlong imageGray, jlong plates, jstring strLisenceNum)
{
    LOGD("Java_com_baidu_trafficplatedetect_TrafficPlateDetect_nativeDetect enter");
    try
    {
    	Mat gray = *((Mat*)imageGray);
    	Rect roi(0, (int)(gray.rows*0.2), gray.cols, (int)(gray.rows*0.6));
    	Mat roiImg = gray(roi);
    	float scale = 1.1;
		Size maxSize(int(180/scale),int(57/scale));
		Size minSize(int(80/scale),int(20/scale));
		Mat smallImg(cvRound(roiImg.rows / scale), cvRound(roiImg.cols / scale), CV_8UC1);
		resize(roiImg, smallImg, smallImg.size(), 0, 0, INTER_LINEAR);

        vector<Rect> RectPlates;
        ((CascadeClassifier*)thiz)->detectMultiScale(smallImg, RectPlates,
        			1.2, 2, 0
        			//|CASCADE_FIND_BIGGEST_OBJECT
        			//|CASCADE_DO_ROUGH_SEARCH
        			| CASCADE_SCALE_IMAGE
        			,
        			minSize, maxSize);

        vector<Rect> RectPlatesTmp;
		for (vector<Rect>::const_iterator r = RectPlates.begin(); r != RectPlates.end(); r++)
		{
			Rect rect;
			rect.x = cvRound(r->x*scale);
			rect.y = cvRound(r->y*scale) + (int)(gray.rows*0.2);
			rect.width = cvRound(r->width*scale);
			rect.height = cvRound(r->height*scale);
			RectPlatesTmp.push_back(rect);
		}

        vector_Rect_to_Mat(RectPlatesTmp, *((Mat*)plates));
    }
    catch(cv::Exception& e)
    {
        LOGD("nativeCreateObject caught cv::Exception: %s", e.what());
        jclass je = jenv->FindClass("org/opencv/core/CvException");
        if(!je)
            je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, e.what());
    }
    catch (...)
    {
        LOGD("nativeDetect caught unknown exception");
        jclass je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, "Unknown exception in JNI code DetectionBasedTracker.nativeDetect()");
    }
    LOGD("Java_com_baidu_trafficplatedetect_TrafficPlateDetect_nativeDetect exit");
}


JNIEXPORT void JNICALL Java_com_baidu_trafficplatedetect_TrafficPlateDetect_nativeDestroyObject
  (JNIEnv *jenv, jclass, jlong thiz)
{
    LOGD("Java_com_baidu_trafficplatedetect_TrafficPlateDetect_nativeDestroyObject enter");
    try
    {
        if(thiz != 0)
        {
            delete (CascadeClassifier*)thiz;
        }
    }
    catch(cv::Exception& e)
    {
        LOGD("nativeestroyObject caught cv::Exception: %s", e.what());
        jclass je = jenv->FindClass("org/opencv/core/CvException");
        if(!je)
            je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, e.what());
    }
    catch (...)
    {
        LOGD("nativeDestroyObject caught unknown exception");
        jclass je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, "Unknown exception in JNI code of DetectionBasedTracker.nativeDestroyObject()");
    }
    LOGD("Java_com_baidu_trafficplatedetect_TrafficPlateDetect_nativeDestroyObject exit");
}
