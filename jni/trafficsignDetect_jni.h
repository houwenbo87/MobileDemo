/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_baidu_trafficsigndetect_TrafficsignDetect */

#ifndef _Included_com_baidu_trafficsigndetect_TrafficsignDetect
#define _Included_com_baidu_trafficsigndetect_TrafficsignDetect
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_baidu_trafficsigndetect_TrafficsignDetect
 * Method:    nativeCreateObject
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_baidu_trafficsigndetect_TrafficsignDetect_nativeCreateObject
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_baidu_trafficsigndetect_TrafficsignDetect
 * Method:    nativeDestroyObject
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_baidu_trafficsigndetect_TrafficsignDetect_nativeDestroyObject
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_baidu_trafficsigndetect_TrafficsignDetect
 * Method:    nativeStart
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_baidu_trafficsigndetect_TrafficsignDetect_nativeStart
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_baidu_trafficsigndetect_TrafficsignDetect
 * Method:    nativeStop
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_baidu_trafficsigndetect_TrafficsignDetect_nativeStop
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_baidu_trafficsigndetect_TrafficsignDetect
 * Method:    nativeDetect
 * Signature: (JJJ)V
 */
JNIEXPORT void JNICALL Java_com_baidu_trafficsigndetect_TrafficsignDetect_nativeDetect
  (JNIEnv *, jclass, jlong, jlong, jlong);

#ifdef __cplusplus
}
#endif
#endif
