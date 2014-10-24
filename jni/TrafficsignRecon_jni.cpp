/*
 * TrafficsignRecon.cpp
 *
 *  Created on: 2014-8-6
 *      Author: houwenbo
 */


#include "TrafficsignRecon_jni.h"
#include "trafficsign/cnn.h"

#include "file_util.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/contrib/detection_based_tracker.hpp>

#include <string>
#include <vector>

#include <android/log.h>

#include "util.h"

using namespace std;
using namespace cv;

inline void vector_Rect_to_Mat(vector<Rect>& v_rect, Mat& mat)
{
    mat = Mat(v_rect, true);
}

/*
 * Class:     com_baidu_trafficsignrecon_TrafficsignRecon
 * Method:    nativeCreateObject
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_baidu_trafficsignrecon_TrafficsignRecon_nativeCreateObject
  (JNIEnv * jenv, jclass obj)
{
    LOGD("Java_com_baidu_trafficsignrecon_TrafficsignRecon_nativeCreateObject enter");

    jlong result = 0;

    try
    {
    	char rootDir[200];
		getExternalRootDir (jenv, obj, rootDir);

		string dataPath = string(rootDir) + "/trafficsign/";

		LOGD("config file path: %s", dataPath.c_str());

        CNN *cnn = new CNN();
        int ret = cnn->initialise(dataPath); //路径先临时写死了
        if (ret != 0)
        {
        	LOGE("TrafficsignRecon_nativeCreateObject init failed!");
        }
        result = (jlong)cnn;
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

    LOGD("Java_com_baidu_trafficsignrecon_TrafficsignRecon_nativeCreateObject exit");
    return result;
}

/*
 * Class:     com_baidu_trafficsignrecon_TrafficsignRecon
 * Method:    nativeDestroyObject
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_baidu_trafficsignrecon_TrafficsignRecon_nativeDestroyObject
  (JNIEnv * jenv, jclass, jlong thiz)
{
    LOGD("Java_com_baidu_trafficsignrecon_TrafficsignRecon_nativeDestroyObject enter");
    try
    {
        if(thiz != 0)
        {
            delete (CNN*)thiz;
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
    LOGD("Java_com_baidu_trafficsignrecon_TrafficsignRecon_nativeDestroyObject exit");
}

/*
 * Class:     com_baidu_trafficsignrecon_TrafficsignRecon
 * Method:    nativeDetect
 * Signature: (JJJ)V
 */
JNIEXPORT jlong JNICALL Java_com_baidu_trafficsignrecon_TrafficsignRecon_nativeDetect
  (JNIEnv * jenv, jclass, jlong thiz, jlong imageRGB)
{
	int signType = 0;
    LOGD("Java_com_baidu_trafficsignrecon_TrafficsignRecon_nativeDetect enter");
    try
    {
    	Mat img = *((Mat*)imageRGB);
    	Mat rgb;
    	cvtColor(img, rgb, CV_BGRA2BGR);
    	Size size(48,48);
    	Mat rzimg;
    	resize(rgb, rzimg, size);
    	vector<double> result;
        int ret = ((CNN*)thiz)->do_cnn(rzimg, result);

        if (ret != 0)
        {
        	LOGE("do_cnn error!");
        }

        //LOGE("cnn return %d", result.size());

        double max = 0;
        int idx = 0;
        for (size_t i = 0; i < result.size(); i++)
        {
        	//LOGE("index %d, score %f", i, result[i]);

        	if (max < result[i])
        	{
        		max = result[i];
        		idx = i;
        	}
        }

        signType = idx;
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
    LOGD("Java_com_baidu_trafficsignrecon_TrafficsignRecon_nativeDetect exit");

    return signType;
}




