LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

MY_FILES := $(wildcard $(LOCAL_PATH)/lane/*.c*)
MY_FILES := $(MY_FILES:$(LOCAL_PATH)/%=%)

MY_FILES_lisenceplate := $(wildcard $(LOCAL_PATH)/lisenceplate/src/*.cpp)
MY_FILES_lisenceplate := $(MY_FILES_lisenceplate:$(LOCAL_PATH)/%=%)


#OPENCV_CAMERA_MODULES:=off
#OPENCV_INSTALL_MODULES:=off
#OPENCV_LIB_TYPE:=SHARED
include ../../sdk/native/jni/OpenCV.mk  

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/lisenceplate/src

LOCAL_SRC_FILES  := DetectionBasedTracker_jni.cpp \
					trafficplatedetect_jni.cpp \
					laneDetectNativeInterface_jni.cpp \
					trafficsign/Cmrnorm.cpp \
					trafficsign/CNN.cpp \
					trafficsign/Conv.cpp \
					trafficsign/FullConnect.cpp \
					trafficsign/Non_linear_cvt.cpp \
					trafficsign/Pool.cpp \
					trafficsignDetect_jni.cpp \
					trafficsignRecon_jni.cpp \
					file_util.cpp \
					TrafficplateRecon_jni.cpp \
					lisenceplate/eblearn/src/bbox.cpp \
					lisenceplate/eblearn/src/carLicensePlate.cpp \
					lisenceplate/eblearn/src/configuration.cpp \
					lisenceplate/eblearn/src/dataset.cpp \
					lisenceplate/eblearn/src/datasource.cpp \
					lisenceplate/eblearn/src/detector.cpp \
					lisenceplate/eblearn/src/ebl_arch.cpp \
					lisenceplate/eblearn/src/ebl_basic.cpp \
					lisenceplate/eblearn/src/ebl_layers.cpp \
					lisenceplate/eblearn/src/ebl_logger.cpp \
					lisenceplate/eblearn/src/ebl_machines.cpp \
					lisenceplate/eblearn/src/ebl_module.cpp \
					lisenceplate/eblearn/src/ebl_nonlinearity.cpp \
					lisenceplate/eblearn/src/ebl_normalization.cpp \
					lisenceplate/eblearn/src/ebl_parameters.cpp \
					lisenceplate/eblearn/src/ebl_state.cpp \
					lisenceplate/eblearn/src/ebl_utils.cpp \
					lisenceplate/eblearn/src/idx.cpp \
					lisenceplate/eblearn/src/idxIO.cpp \
					lisenceplate/eblearn/src/idxops.cpp \
					lisenceplate/eblearn/src/idxspec.cpp \
					lisenceplate/eblearn/src/image.cpp \
					lisenceplate/eblearn/src/imageIO.cpp \
					lisenceplate/eblearn/src/netconf.cpp \
					lisenceplate/eblearn/src/nms.cpp \
					lisenceplate/eblearn/src/numerics.cpp \
					lisenceplate/eblearn/src/random.cpp \
					lisenceplate/eblearn/src/smart.cpp \
					lisenceplate/eblearn/src/srg.cpp \
					lisenceplate/eblearn/src/stl.cpp \
					lisenceplate/eblearn/src/string_utils.cpp \
					lisenceplate/eblearn/src/thread.cpp \
					lisenceplate/eblearn/src/tools_utils.cpp \
					lisenceplate/eblearn/src/utils.cpp
					
LOCAL_SRC_FILES += $(MY_FILES) 
LOCAL_SRC_FILES += $(MY_FILES_lisenceplate) 

#LOCAL_LDFLAGS += $(LOCAL_PATH)/lib/libidx.a
#LOCAL_LDFLAGS += $(LOCAL_PATH)/lib/libeblearntools.a
#LOCAL_LDFLAGS += $(LOCAL_PATH)/lib/libeblearn.a


#LOCAL_LDLIBS     += -llog -ldl -leblearn -leblearntools -lidx -L$(LOCAL_PATH)/lib
LOCAL_LDLIBS     += -llog -ldl

LOCAL_MODULE     := detection_based_tracker

include $(BUILD_SHARED_LIBRARY)
