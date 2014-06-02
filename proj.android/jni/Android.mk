LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

CLASSES_DIRECTORY := $(LOCAL_PATH)/../../Classes

# Get a list of source files
SOURCE_FILES := $(shell find $(CLASSES_DIRECTORY) -name *.cpp)
SOURCE_FILES := $(sort $(SOURCE_FILES))
SOURCE_FILES := $(subst $(LOCAL_PATH)/,,$(SOURCE_FILES))

LOCAL_SRC_FILES := hellocpp/main.cpp \
			$(SOURCE_FILES)
#                   ../../Classes/AppDelegate.cpp \
#                   ../../Classes/intro/BPIntroLayer.cpp


LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes \
					$(LOCAL_PATH)/../../Classes/game \
					$(LOCAL_PATH)/../../Classes/intro \
					$(LOCAL_PATH)/../../Classes/ITGLib/network \
					$(LOCAL_PATH)/../../Classes/ITGLib/node \
					$(LOCAL_PATH)/../../Classes/ITGLib/utils \
					


LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static
LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static
LOCAL_WHOLE_STATIC_LIBRARIES += chipmunk_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static

LOCAL_CPPFLAGS := -fexceptions -std=c++11

include $(BUILD_SHARED_LIBRARY)

$(call import-module,cocos2dx)
$(call import-module,cocos2dx/platform/third_party/android/prebuilt/libcurl)
$(call import-module,CocosDenshion/android)
$(call import-module,extensions)
$(call import-module,external/Box2D)
$(call import-module,external/chipmunk)
