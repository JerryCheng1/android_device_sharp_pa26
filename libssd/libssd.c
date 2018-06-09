/*
 * Copyright (C) 2016 The CyanogenMod Project
 *               2017 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "SSD"
#define LOG_NDEBUG 0

#include <cutils/log.h>

#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define LIB_QSEECOMAPI "libQSEEComAPI.so"

pthread_t id;

struct QSEECom_handle {
	unsigned char *ion_sbuffer;
};

struct QSEECom_handle * mHandle;

typedef int (*QSEECom_register_listener_t)(struct QSEECom_handle **handle,
			uint32_t lstnr_id, uint32_t sb_length, uint32_t flags);

typedef int (*QSEECom_unregister_listener_t)(struct QSEECom_handle *handle);

int ssd_init_service()
{
	int result = 0;
	ALOGV("ssd_init_service succeeded!\n");
	return result;
}

void* fake_ssd_start()
{
	ALOGV("ssd start succeeded!\n");
	return((void*)0);
}

int ssd_start(uint32_t lstnr_id)
{
	int result;
	mHandle = 0;

	void* handle = dlopen(LIB_QSEECOMAPI, RTLD_NOW);

	if (!handle) {
		ALOGE("%s", dlerror());
		result = -1;
		return result;
	}

	QSEECom_register_listener_t QSEECom_register_listener =
		(QSEECom_register_listener_t)dlsym(handle, "QSEECom_register_listener");

	if (!QSEECom_register_listener) {
		ALOGE("%s", dlerror());
		result = -1;
		return result;
	}

	result = QSEECom_register_listener(&mHandle, lstnr_id, 20480, 0);
	if ( result == -2 )
		return 0;
	if ( result == -1 )
		return result;

	if ( !pthread_create(&id, NULL, fake_ssd_start, NULL) )
		return 0;

	ALOGE("Error: Creating a pthread in ssd_start is failed!");
	return -1;
}

int ssd_close()
{
	int ret;
	int result = 0;
	mHandle = 0;

	void* handle = dlopen(LIB_QSEECOMAPI, RTLD_NOW);
	if (!handle) {
		ALOGE("%s", dlerror());
		result = -1;
		return result;
	}

	QSEECom_unregister_listener_t QSEECom_unregister_listener =
		(QSEECom_unregister_listener_t)dlsym(handle, "QSEECom_unregister_listener");

	if (!QSEECom_unregister_listener) {
		ALOGE("%s", dlerror());
		result = -1;
		return result;
	}

	ret = QSEECom_unregister_listener(mHandle);

	if (ret == -1) {
		ALOGE("Error: ssd unregister call failed!");
	} else {
		pthread_join(id, 0);
		result = ret;
	}

	return result;
}
