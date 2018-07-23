LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE             := libssd
LOCAL_MODULE_TAGS        := optional
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SRC_FILES          := libssd.c
LOCAL_CFLAGS             += -Wall -Werror
LOCAL_SHARED_LIBRARIES   := liblog
include $(BUILD_SHARED_LIBRARY)
