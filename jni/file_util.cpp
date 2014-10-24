
/* DO NOT EDIT THIS FILE - it is machine generated */
#include <stdio.h>
#include <stdlib.h>
#include <jni.h>
#include <android/log.h>

#include "util.h"

/*
 * Class:     com_example_nativefileio_FileSave
 * Method:    save
 * Signature: ()V
 */
#ifdef __cplusplus
extern "C" {
#endif
void getExternalRootDir (JNIEnv * env, jobject obj, char* rootDir)
{
	jclass fileUtilClass = env->FindClass("com/baidu/detector/util/FileUtil");
	jmethodID getPathMethodId = env->GetStaticMethodID(fileUtilClass, "getSDCardPath", "()Ljava/lang/String;");
	jstring s = (jstring)(env->CallStaticObjectMethod(fileUtilClass, getPathMethodId));
	const char* rootDirm = env->GetStringUTFChars(s, NULL);
	strcpy(rootDir, rootDirm);
	//rootDir = rootDirm;
	LOGD("getExternalRootDir: %s", rootDir);
	env->ReleaseStringUTFChars(s, rootDirm);
}
#ifdef __cplusplus
}
#endif

