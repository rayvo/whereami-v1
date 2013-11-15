LOCAL_PATH := $(call my-dir)

STL_LIBS := -L/home/vqd/ndk/sources/cxx-stl/gnu-libstdc++/4.6/libs/armeabi/include
                    
include $(CLEAR_VARS)

OPENCV_LIB_TYPE:=STATIC
OPENCV_INSTALL_MODULES:=on

include /home/vqd/workspace/sources/OpenCV-2.4.6-android-sdk/sdk/native/jni/OpenCV.mk
ifeq ("$(wildcard $(OPENCV_MK_PATH))","")
	#try to load OpenCV.mk from default install location
	include /home/vqd/workspace/sources/OpenCV-2.4.6-android-sdk/sdk/native/jni/OpenCV.mk
else
	include $(OPENCV_MK_PATH)
endif

LOCAL_MODULE    := ImageProcessing
LOCAL_SRC_FILES := ImageProcessing.cpp FeaturesMain.cpp TextDetection.cpp TextDetection.h

LOCAL_CFLAGS += -I$(LOCAL_PATH)/boost/include/boost-1_53

LOCAL_LDLIBS +=  -llog -ldl -ljnigraphics

LOCAL_CPPFLAGS += -fexceptions
LOCAL_CPPFLAGS += -frtti

include $(BUILD_SHARED_LIBRARY)