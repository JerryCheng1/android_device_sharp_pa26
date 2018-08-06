/*
 * Copyright (C) 2015, The Android Open Source Project
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

/**
* @file CameraWrapper.cpp
*
* This file wraps a vendor camera module.
*
*/

#define LOG_NDEBUG 1

#define LOG_TAG "CameraWrapper"
#include <cutils/log.h>

#include <utils/threads.h>
#include <utils/String8.h>
#include <hardware/hardware.h>
#include <hardware/camera.h>
#include <camera/Camera.h>
#include <camera/CameraParameters.h>

static android::Mutex gCameraWrapperLock;
static camera_module_t *gVendorModule = 0;

//static char KEY_QC_MORPHO_HDR[] = "morpho-hdr";
//static char KEY_QC_CHROMA_FLASH[] = "chroma-flash";
//static char CHROMA_FLASH_ON[] = "chroma-flash-on";
//static char CHROMA_FLASH_OFF[] = "chroma-flash-off";

static char CONTINUOUS_COUNT_8PICS[] = "8pics";
static char KEY_MAX_NUM_METERING_AREAS[] = "max-num-metering-areas";
static char KEY_AUTO_WHITEBALANCE_LOCK[] = "auto-whitebalance-lock";
static char KEY_FOCUS_DISTANCES[] = "focus-distances";
static char FOCUS_MODE_CONTINUOUS_PICTURE[] = "continuous-picture";
static char KEY_MAX_EXPOSURE_COMPENSATION[] = "max-exposure-compensation";
static char KEY_DIG_ZOOM_RATIOS[] = "digital-zoom-ratios";
static char KEY_BULB[] = "bulb";
static char VIDEO_HFR_4X[] = "120";
static char KEY_AUTO_EXPOSURE_LOCK_SUPPORTED[] = "auto-exposure-lock-supported";
static char SCENE_MODE_NIGHT[] = "night";
static char KEY_MULTI_FOCUS[] = "multi-focus";
static char KEY_QR_DETECT[] = "qr-detect";
static char ASPECT_3D_LANDSCAPE[] = "landscape";
static char KEY_RECORDING_HINT[] = "recording-hint";
static char KEY_DYNAMIC_RANGE[] = "dynamic-range";
static char WHITE_BALANCE_FLUORESCENT[] = "fluorescent";
static char KEY_SUPPORTED_SCENE_MODES[] = "scene-mode-values";
static char SCENE_MODE_QRCODE[] = "qrcode";
static char EFFECT_NONE[] = "none";
static char GOLIATH_PREVIEW_OFF[] = "off";
static char CONTINUOUS_COUNT_40PICS[] = "40pics";
static char KEY_SH_DENOISE[] = "Sh-denoise";
static char LIGHTFX_HDR[] = "high-dynamic-range";
static char SCENE_MODE_BABY[] = "baby";
static char KEY_SUPPORTED_PREVIEW_FPS_RANGE[] = "preview-fps-range-values";
static char FOCUS_MODE_FIXED[] = "fixed";
static char ANTIBANDING_AUTO[] = "auto";
static char KEY_SUPPORTED_PICTURE_SIZES[] = "picture-size-values";
static char VIDEO_HFR_2X[] = "60";
static char CONTINUOUS_COUNT_18PICS[] = "18pics";
static char NIGHT_SHOT_OFF[] = "0";
static char FOCUS_MODE_AUTO[] = "auto";
static char FOCUS_MODE_MANUAL_POSITION[] = "manual";
static char KEY_FOCUS_AREAS[] = "focus-areas.";
static char HUTTER_SOUND_PATTERN_1[] = "pattern1";
static char KEY_CURRENT_PICTURE_SIZE_HEIGHT[] = "current-picture-size-height";
static char SCENE_MODE_STEADYPHOTO[] = "steadyphoto";
static char EFFECT_BLACKBOARD[] = "blackboard";
static char KEY_FACE_BACKLIGHT[] = "face-backlight";
static char SCENE_MODE_LANDSCAPE[] = "landscape";
static char ASPECT_PORTRAIT[] = "portrait";
static char KEY_MIN_EXPOSURE_COMPENSATION[] = "min-exposure-compensation";
static char KEY_JPEG_THUMBNAIL_WIDTH[] = "jpeg-thumbnail-width";
static char KEY_AUTO_WHITEBALANCE_LOCK_SUPPORTED[] = "auto-whitebalance-lock-supported";
static char DYNAMIC_RANGE_OFF[] = "off";
static char CONTINUOUS_COUNT_9PICS[] = "9pics";
static char WHITE_BALANCE_MANUAL_CCT[] = "manual-cct";
static char KEY_CAMERA_MODE[] = "camera-mode";
static char KEY_EXTRA_PICTURE_SIZE[] = "extra-picture-size-values";
static char WHITE_BALANCE_INCANDESCENT[] = "incandescent";
static char WHITE_BALANCE_TWILIGHT[] = "twilight";
static char KEY_MAX_SENSOR_ZOOM[] = "sensor-max-zoom";
static char EFFECT_POSTERIZE[] = "posterize";
static char FOCUS_MODE_EDOF[] = "edof";
static char EFFECT_WHITEBOARD[] = "whiteboard";
static char FOCUS_MODE_MACRO[] = "macro";
static char KEY_GOLIATH_PREVIEW_OFFSET_TOP[] = "goliath-preview-offset-top";
static char ANTIBANDING_50HZ[] = "50hz";
static char KEY_FACE_ROTATION[] = "face-rotation";
static char KEY_CURRENT_PICTURE_SIZE_WIDTH[] = "current-picture-size-width";
static char EFFECT_MONO[] = "mono";
static char FACE_FINDER_MODE_TURNAROUND[] = "turn-around";
static char SHUTTER_SOUND_PATTERN_2[] = "pattern2";
static char SCENE_MODE_BACKLIGHT[] = "backlight";
static char QR_DETECT_ON[] = "1";
static char VIDEO_HFR_OFF[] = "off";
static char ASPECT_LANDSCAPE[] = "landscape";
static char LIGHTFX_LOWLIGHT[] = "low-light";
static char PIXEL_FORMAT_JPEG[] = "jpeg";
static char SCENE_MODE_HDR[] = "hdr";
static char KEY_SUPPORTED_EFFECTS[] = "effect-values";
static char CONTINUOUS_COUNT_22PICS[] = "22pics";
static char SCENE_MODE_SILHOUETTE[] = "silhouette";
static char SCENE_MODE_SLEEPINGFACE[] = "sleepingface";
static char SCENE_MODE_BARCODE[] = "barcode";
static char PIXEL_FORMAT_RGBA8888[] = "rgba8888";
static char FLASH_MODE_TORCH[] = "torch";
static char FOCUS_MODE_CAF[] = "caf";
static char KEY_GPS_TIMESTAMP[] = "gps-timestamp";
static char FACE_FINDER_MODE_OFF[] = "off";
static char ISO_AUTO_SUPERHIGH[] = "auto-superhigh";
static char KEY_SUPER_MIN_ZOOM[] = "super-min-zoom";
static char CONTINUOUS_MODE_NORMAL[] = "normal";
static char KEY_SUPPORTED_PREVIEW_FORMATS[] = "preview-format-values";
static char CAMERA_MODE_AUTO_CHANGE_ON[] = "on";
static char KEY_SHUTTER_SOUND[] = "shutter-sound";
static char KEY_JPEG_QUALITY[] = "jpeg-quality";
static char SUPER_ZOOM_OFF[] = "0";
static char KEY_AUTO_EXPOSURE_LOCK[] = "auto-exposure-lock";
static char KEY_EFFECT[] = "effect";
static char FACE_BACKLIGHT_ON[] = "on";
static char KEY_SUPPORTED_THUMBNAIL_SIZES[] = "jpeg-thumbnail-size-values";
static char EFFECT_AQUA[] = "aqua";
static char KEY_SUPPORTED_CONTINUOUS_AF[] = "continuous-af-values";
static char CAMERA_MODE_AUTO_CHANGE_OFF[] = "off";
static char KEY_SMOOTH_ZOOM_SUPPORTED[] = "smooth-zoom-supported";
static char SHUTTER_SOUND_PATTERN_3[] = "pattern3";
static char KEY_NIGHT_SHOT[] = "night-shot";
static char KEY_VIDEO_SIZE[] = "video-size";
static char PIXEL_FORMAT_RGB565[] = "rgb565";
static char KEY_FACE_FINDER_MODE[] = "face-finder-mode";
static char SCENE_MODE_NAMECARD[] = "namecard";
static char WHITE_BALANCE_DAYLIGHT[] = "daylight";
static char ISO_6400[] = "ISO6400";
static char SCENE_MODE_NORMAL[] = "auto";
static char SCENE_MODE_AUTO[] = "auto";
static char KEY_SINGLE_ISP_OUTPUT_ENABLED[] = "single-isp-output-enabled";
static char KEY_SUPPORTED_ANTIBANDING[] = "antibanding-values";
static char KEY_SUPPORTED_PREVIEW_FRAME_RATES[] = "preview-frame-rate-values";
static char KEY_JPEG_THUMBNAIL_HEIGHT[] = "jpeg-thumbnail-height";
static char CONTINUOUS_COUNT_15PICS[] = "15pics";
static char SUPER_ZOOM_ON[] = "1";
static char KEY_SUPPORTED_WHITE_BALANCE[] = "whitebalance-values";
static char KEY_EXPOSURE_COMPENSATION_STEP[] = "exposure-compensation-step";
static char KEY_MULTI_FOCUS_LENS_POSITIONS[] = "multi-focus-lens-positions";
static char ISO_AUTO_HIGH[] = "auto-high";
static char SCENE_MODE_SPORTS[] = "sports";
static char SCENE_MODE_SNOW[] = "snow";
static char ASPECT_3D_PORTRAIT[] = "portrait";
static char PREVIEW_FRAME_RATE_AUTO_MODE[] = "frame-rate-auto";
static char SCENE_MODE_TURNRED[] = "turnred";
static char SCENE_MODE_PET[] = "pet";
static char SCENE_MODE_ACTION[] = "action";
static char FLASH_MODE_OFF[] = "off";
static char KEY_SUPPORTED_FOCUS_MODES[] = "focus-mode-values";
static char KEY_MAX_NUM_FOCUS_AREAS[] = "max-num-focus-areas";
static char ANTIBANDING_OFF[] = "off";
static char KEY_ZSL_ON[] = "on";
static char KEY_3D_ASPECT[] = "3daspect";
static char PIXEL_FORMAT_YUV420P[] = "yuv420p";
static char KEY_MAX_ZOOM[] = "max-zoom";
static char SHUTTER_SOUND_PATTERN_4[] = "pattern4";
static char KEY_QC_SUPPORTED_CAMERA_FEATURES[] = "qc-camera-features";
static char KEY_SUPPORTED_PICTURE_FORMATS[] = "picture-format-values";
static char SCENE_MODE_PARTY[] = "party";
static char CONTINUOUS_COUNT_10PICS[] = "10pics";
static char KEY_GPS_LATITUDE[] = "gps-latitude";
static char SCENE_MODE_SMILE[] = "smile";
static char KEY_LIGHTFX[] = "light-fx";
static char KEY_METERING_AREAS[] = "metering-areas";
static char CONTINUOUS_COUNT_30PICS[] = "30pics";
static char KEY_CONTINUOUS_COUNT[] = "continuous-count";
static char FOCUS_MODE_INFINITY[] = "infinity";
static char SCENE_MODE_FOOD[] = "food";
static char KEY_SUPPORTED_FLASH_MODES[] = "flash-mode-values";
static char CONTINUOUS_MODE_IMAGESTABILIZER_2[] = "tebure_2";
static char FLASH_MODE_RED_EYE[] = "red-eye";
static char KEY_CONTINUOUS_MODE[] = "continuous";
static char SCENE_MODE_FIREWORKS[] = "fireworks";
static char WHITE_BALANCE_CLOUDY_DAYLIGHT[] = "cloudy-daylight";
static char CONTINUOUS_COUNT_50PICS[] = "50pics";
static char CONTINUOUS_MODE_IMAGESTABILIZER[] = "tebure";
static char OIS_MODE_OFF[] = "off";
static char KEY_ZSL[] = "zsl";
static char FACE_FINDER_MODE_NORMAL[] = "normal";
static char KEY_SUPER_ZOOM[] = "super-zoom";
static char DENOISE_OFF[] = "denoise-off";
static char KEY_ZSL_OFF[] = "off";
static char SCENE_MODE_AUTO_SW_DETECT[] = "auto_sw_detec";
static char ISO_12800[] = "ISO12800";
static char FOCUS_MODE_CAF_PAUSE[] = "caf-pause";
static char KEY_ZOOM_SUPPORTED[] = "zoom-supported";
static char KEY_VIDEO_STABILIZATION[] = "video-stabilization";
static char KEY_SUPPORTED_PREVIEW_SIZES[] = "preview-size-values";
static char KEY_VIDEO_FRAME_FORMAT[] = "video-frame-format";
static char EFFECT_SEPIA[] = "sepia";
static char KEY_PREVIEW_SIZE[] = "preview-size";
static char KEY_PREVIEW_FPS_RANGE[] = "preview-fps-range";
static char FACE_FINDER_MODE_SMILE[] = "smile";
static char SHUTTER_SOUND_PATTERN_5[] = "pattern5";
static char KEY_CAMERA_MODE_AUTO_CHANGE[] = "camera-mode-auto-change";
static char KEY_SUPPORTED_OIS_MODES[] = "ois-mode-values";
static char RED_LED_ON[] = "on";
static char FOCUS_MODE_CONTINUOUS_VIDEO[] = "continuous-video";
static char DYNAMIC_RANGE_ON[] = "on";
static char EFFECT_NEON[] = "neon";
static char FACE_BACKLIGHT_OFF[] = "off";
static char KEY_ASPECT[] = "aspect";
static char CONTINUOUS_COUNT_5PICS[] = "5pics";
static char KEY_GOLIATH_PREVIEW[] = "goliath-preview";
static char SCENE_MODE_PORTRAIT_IN[] = "portrait_in";
static char SCENE_MODE_FLOWERS[] = "flowers";
static char KEY_PICTURE_SIZE[] = "picture-size";
static char PIXEL_FORMAT_YUV422SP[] = "yuv422sp";
static char KEY_GOLIATH_PREVIEW_OFFSET_LEFT[] = "goliath-preview-offset-left";
static char KEY_SELF_TIMER[] = "self-timer";
static char KEY_MAX_DIG_ZOOM[] = "digital-max-zoom";
static char KEY_HORIZONTAL_VIEW_ANGLE[] = "horizontal-view-angle";
static char FLASH_MODE_AUTO[] = "auto";
static char PIXEL_FORMAT_BAYER_RGGB[] = "bayer-rggb";
static char SCENE_MODE_PORTRAIT[] = "portrait";
static char CONTINUOUS_AF_OFF[] = "caf-off";
static char KEY_ZOOM_RATIOS[] = "zoom-ratios";
static char KEY_VIDEO_STABILIZATION_SUPPORTED[] = "video-stabilization-supported";
static char KEY_PREVIEW_FORMAT[] = "preview-format";
static char MULTI_FOCUS_ON[] = "1";
static char KEY_BRIGHTNESS[] = "brightness";
static char ISO_3200[] = "ISO3200";
static char KEY_VERTICAL_VIEW_ANGLE[] = "vertical-view-angle";
static char KEY_FLASH_MODE[] = "flash-mode";
static char OIS_MODE_ON[] = "on";
static char KEY_OIS_MODE[] = "ois-mode";
static char CONTINUOUS_COUNT_12PICS[] = "12pics";
static char FLASH_MODE_ON[] = "on";
static char KEY_ZOOM[] = "zoom";
static char CONTINUOUS_MODE_LOW[] = "low";
static char SCENE_MODE_SUNSET[] = "sunset";
static char KEY_VIDEO_SNAPSHOT_SUPPORTED[] = "video-snapshot-supported";
static char KEY_ROTATION[] = "rotation";
static char KEY_QC_MAX_NUM_REQUESTED_FACES[] = "qc-max-num-requested-faces";
static char SCENE_MODE_THEATRE[] = "theatre";
static char KEY_SUPPORTED_VIDEO_SIZES[] = "video-size-values";
static char KEY_PREFERRED_PREVIEW_SIZE_FOR_VIDEO[] = "preferred-preview-size-for-video";
static char KEY_GPS_PROCESSING_METHOD[] = "gps-processing-method";
static char SCENE_MODE_LETTER[] = "letter";
static char EFFECT_SOLARIZE[] = "solarize";
static char SCENE_MODE_BEACH[] = "beach";
static char PIXEL_FORMAT_ANDROID_OPAQUE[] = "android-opaque";
static char KEY_CONTINUOUS_AF[] = "continuous-af";
static char KEY_SUPPORTED_JPEG_THUMBNAIL_SIZES[] = "jpeg-thumbnail-size-values";
static char CONTINUOUS_MODE_OFF[] = "ren_off";
static char KEY_PICTURE_FORMAT[] = "picture-format";
static char KEY_VIDEO_HIGH_FRAME_RATE[] = " video-hfr";
static char PREVIEW_FRAME_RATE_FIXED_MODE[] = "frame-rate-fixed";
static char SCENE_MODE_NIGHT_PORTRAIT[] = "night-portrait";
static char KEY_SCENE_MODE[] = "scene-mode";
static char WHITE_BALANCE_SHADE[] = "shade";
static char ANTIBANDING_60HZ[] = "60hz";
static char CONTINUOUS_COUNT_16PICS[] = "16pics";
static char KEY_EXPOSURE_COMPENSATION[] = "exposure-compensation";
static char KEY_MAX_NUM_DETECTED_FACES_HW[] = "max-num-detected-faces-hw";
static char WHITE_BALANCE_AUTO[] = "auto";
static char KEY_GPS_LONGITUDE[] = "gps-longitude";
static char SCENE_MODE_CANDLELIGHT[] = "candlelight";
static char KEY_DIG_ZOOM[] = "digital-zoom";
static char KEY_MAX_NUM_DETECTED_FACES_SW[] = "max-num-detected-faces-sw";
static char KEY_JPEG_THUMBNAIL_QUALITY[] = "jpeg-thumbnail-quality";
static char CONTINUOUS_COUNT_4PICS[] = "4pics";
static char KEY_RED_LED[] = "red_led";
static char PIXEL_FORMAT_YUV422I[] = "yuv422i-yuyv";
static char DENOISE_ON[] = "denoise-on";
static char NIGHT_SHOT_AUTO[] = "1";
static char PIXEL_FORMAT_YUV420SP[] = "yuv420sp";
static char KEY_WHITE_BALANCE[] = "whitebalance";
static char QR_DETECT_OFF[] = "0";
static char KEY_PREVIEW_FRAME_RATE[] = "preview-frame-rate";
static char VIDEO_HFR_3X[] = "90";
static char SCENE_MODE_PORTRAIT_OUT[] = "portrait_out";
static char KEY_GPS_ALTITUDE[] = "gps-altitude";
static char KEY_ANTIBANDING[] = "antibanding";
static char RED_LED_OFF[] = "off";
static char MULTI_FOCUS_OFF[] = "0";
static char EFFECT_BLEACHBYPASS[] = "bleachbypass";
static char CONTINUOUS_COUNT_25PICS[] = "25pics";
static char CONTINUOUS_AF_ON[] = "caf-on";
static char CONTINUOUS_MODE_FAST[] = "fast";
static char KEY_FOCUS_MODE[] = "focus-mode";
static char GOLIATH_PREVIEW_ON[] = "on";
static char EFFECT_NEGATIVE[] = "negative";
static char KEY_FOCAL_LENGTH[] = "focal-length";
static char WHITE_BALANCE_WARM_FLUORESCENT[] = "warm-fluorescent";
static char KEY_SCENE_ROTATION[] = "scene-rotation";
//static char KEY_QC_CAMERA_MODE[] = "camera-mode";
static char **fixed_set_params = NULL;

static int camera_device_open(const hw_module_t *module, const char *name,
        hw_device_t **device);
static int camera_get_number_of_cameras(void);
static int camera_get_camera_info(int camera_id, struct camera_info *info);

static struct hw_module_methods_t camera_module_methods = {
    .open = camera_device_open
};

camera_module_t HAL_MODULE_INFO_SYM = {
    .common = {
         .tag = HARDWARE_MODULE_TAG,
         .module_api_version = CAMERA_MODULE_API_VERSION_1_0,
         .hal_api_version = HARDWARE_HAL_API_VERSION,
         .id = CAMERA_HARDWARE_MODULE_ID,
         .name = "PA26 Unofficial Camera Wrapper",
         .author = "The Android Open Source Project",
         .methods = &camera_module_methods,
         .dso = NULL, /* remove compilation warnings */
         .reserved = {0}, /* remove compilation warnings */
    },
    .get_number_of_cameras = camera_get_number_of_cameras,
    .get_camera_info = camera_get_camera_info,
    .set_callbacks = NULL, /* remove compilation warnings */
    .get_vendor_tag_ops = NULL, /* remove compilation warnings */
    .open_legacy = NULL, /* remove compilation warnings */
    .set_torch_mode = NULL, /* remove compilation warnings */
    .init = NULL, /* remove compilation warnings */
    .reserved = {0}, /* remove compilation warnings */
};

typedef struct wrapper_camera_device {
    camera_device_t base;
    int id;
    camera_device_t *vendor;
} wrapper_camera_device_t;

#define VENDOR_CALL(device, func, ...) ({ \
    wrapper_camera_device_t *__wrapper_dev = (wrapper_camera_device_t*) device; \
    __wrapper_dev->vendor->ops->func(__wrapper_dev->vendor, ##__VA_ARGS__); \
})

#define CAMERA_ID(device) (((wrapper_camera_device_t *)(device))->id)

static int check_vendor_module()
{
    int rv = 0;
    ALOGV("%s", __FUNCTION__);

    if (gVendorModule)
        return 0;

    rv = hw_get_module_by_class("camera", "vendor",
            (const hw_module_t**)&gVendorModule);
    if (rv)
        ALOGE("failed to open vendor camera module");
    return rv;
}

static char *camera_fixup_getparams(int id __attribute__((unused)),
        const char *settings)
{
    android::CameraParameters params;
    params.unflatten(android::String8(settings));

#if !LOG_NDEBUG
    ALOGV("%s: Original parameters:", __FUNCTION__);
    params.dump();
#endif

#if !LOG_NDEBUG
    ALOGV("%s: Fixed parameters:", __FUNCTION__);
    params.dump();
#endif

    android::String8 strParams = params.flatten();
    char *ret = strdup(strParams.string());

    return ret;
}

static char *camera_fixup_setparams(int id, const char *settings)
{
    bool videoMode = false;
    bool hdrMode = false;

    android::CameraParameters params;
    params.unflatten(android::String8(settings));

#if !LOG_NDEBUG
    ALOGV("%s: original parameters:", __FUNCTION__);
    params.dump();
#endif

    params.set(android::CameraParameters::KEY_VIDEO_STABILIZATION, "false");

    /* ZSL
    if (params.get(android::CameraParameters::KEY_RECORDING_HINT)) {
        videoMode = !strcmp(params.get(android::CameraParameters::KEY_RECORDING_HINT), "true");
    }*/

    /* HDR */
    if (params.get(android::CameraParameters::KEY_SCENE_MODE)) {
        hdrMode = (!strcmp(params.get(android::CameraParameters::KEY_SCENE_MODE), "hdr"));
    }
    if (hdrMode) {
        params.set(KEY_QC_MORPHO_HDR, "true");
        params.set(android::CameraParameters::KEY_FLASH_MODE, android::CameraParameters::FLASH_MODE_OFF);
        params.set("ae-bracket-hdr", "AE-Bracket");
        params.set("capture-burst-exposures", "-6,8,0");

        // enable ZSL only when HDR is on, otherwise some camera apps will break
        /*params.set("zsl", "on");
        params.set(KEY_QC_CAMERA_MODE, "1");*/
    } else {
        params.set(KEY_QC_MORPHO_HDR, "false");
        params.set("ae-bracket-hdr", "Off");
        params.set("capture-burst-exposures", "0,0,0");
        //params.set("zsl", "off");
        //params.set(KEY_QC_CAMERA_MODE, "0");
    }

    // force ZSL off for videos
    /*if (videoMode)
        params.set("zsl", "off");*/


#if !LOG_NDEBUG
    ALOGV("%s: fixed parameters:", __FUNCTION__);
    params.dump();
#endif

    android::String8 strParams = params.flatten();
    if (fixed_set_params[id])
        free(fixed_set_params[id]);
    fixed_set_params[id] = strdup(strParams.string());
    char *ret = fixed_set_params[id];

    return ret;
}

/*******************************************************************
 * implementation of camera_device_ops functions
 *******************************************************************/

static int camera_set_preview_window(struct camera_device *device,
        struct preview_stream_ops *window)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, set_preview_window, window);
}

static void camera_set_callbacks(struct camera_device *device,
        camera_notify_callback notify_cb,
        camera_data_callback data_cb,
        camera_data_timestamp_callback data_cb_timestamp,
        camera_request_memory get_memory,
        void *user)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return;

    VENDOR_CALL(device, set_callbacks, notify_cb, data_cb, data_cb_timestamp,
            get_memory, user);
}

static void camera_enable_msg_type(struct camera_device *device,
        int32_t msg_type)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return;

    VENDOR_CALL(device, enable_msg_type, msg_type);
}

static void camera_disable_msg_type(struct camera_device *device,
        int32_t msg_type)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return;

    VENDOR_CALL(device, disable_msg_type, msg_type);
}

static int camera_msg_type_enabled(struct camera_device *device,
        int32_t msg_type)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return 0;

    return VENDOR_CALL(device, msg_type_enabled, msg_type);
}

static int camera_start_preview(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, start_preview);
}

static void camera_stop_preview(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return;

    VENDOR_CALL(device, stop_preview);
}

static int camera_preview_enabled(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, preview_enabled);
}

static int camera_store_meta_data_in_buffers(struct camera_device *device,
        int enable)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, store_meta_data_in_buffers, enable);
}

static int camera_start_recording(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return EINVAL;

    return VENDOR_CALL(device, start_recording);
}

static void camera_stop_recording(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return;

    VENDOR_CALL(device, stop_recording);
}

static int camera_recording_enabled(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, recording_enabled);
}

static void camera_release_recording_frame(struct camera_device *device,
        const void *opaque)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return;

    VENDOR_CALL(device, release_recording_frame, opaque);
}

static int camera_auto_focus(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;


    return VENDOR_CALL(device, auto_focus);
}

static int camera_cancel_auto_focus(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, cancel_auto_focus);
}

static int camera_take_picture(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, take_picture);
}

static int camera_cancel_picture(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, cancel_picture);
}

static int camera_set_parameters(struct camera_device *device,
        const char *params)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    char *tmp = NULL;
    tmp = camera_fixup_setparams(CAMERA_ID(device), params);

    int ret = VENDOR_CALL(device, set_parameters, tmp);
    return ret;
}

static char *camera_get_parameters(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return NULL;

    char *params = VENDOR_CALL(device, get_parameters);

    char *tmp = camera_fixup_getparams(CAMERA_ID(device), params);
    VENDOR_CALL(device, put_parameters, params);
    params = tmp;

    return params;
}

static void camera_put_parameters(struct camera_device *device, char *params)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (params)
        free(params);
}

static int camera_send_command(struct camera_device *device,
        int32_t cmd, int32_t arg1, int32_t arg2)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, send_command, cmd, arg1, arg2);
}

static void camera_release(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return;

    VENDOR_CALL(device, release);
}

static int camera_dump(struct camera_device *device, int fd)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, dump, fd);
}

extern "C" void heaptracker_free_leaked_memory(void);

static int camera_device_close(hw_device_t *device)
{
    int ret = 0;
    wrapper_camera_device_t *wrapper_dev = NULL;

    ALOGV("%s", __FUNCTION__);

    android::Mutex::Autolock lock(gCameraWrapperLock);

    if (!device) {
        ret = -EINVAL;
        goto done;
    }

    for (int i = 0; i < camera_get_number_of_cameras(); i++) {
        if (fixed_set_params[i])
            free(fixed_set_params[i]);
    }

    wrapper_dev = (wrapper_camera_device_t*) device;

    wrapper_dev->vendor->common.close((hw_device_t*)wrapper_dev->vendor);
    if (wrapper_dev->base.ops)
        free(wrapper_dev->base.ops);
    free(wrapper_dev);
done:
#ifdef HEAPTRACKER
    heaptracker_free_leaked_memory();
#endif
    return ret;
}

/*******************************************************************
 * implementation of camera_module functions
 *******************************************************************/

/* open device handle to one of the cameras
 *
 * assume camera service will keep singleton of each camera
 * so this function will always only be called once per camera instance
 */

static int camera_device_open(const hw_module_t *module, const char *name,
        hw_device_t **device)
{
    int rv = 0;
    int num_cameras = 0;
    int cameraid;
    wrapper_camera_device_t *camera_device = NULL;
    camera_device_ops_t *camera_ops = NULL;

    android::Mutex::Autolock lock(gCameraWrapperLock);

    ALOGV("%s", __FUNCTION__);

    if (name != NULL) {
        if (check_vendor_module())
            return -EINVAL;

        cameraid = atoi(name);
        num_cameras = gVendorModule->get_number_of_cameras();

        fixed_set_params = (char **) malloc(sizeof(char *) * num_cameras);
        if (!fixed_set_params) {
            ALOGE("parameter memory allocation fail");
            rv = -ENOMEM;
            goto fail;
        }
        memset(fixed_set_params, 0, sizeof(char *) * num_cameras);

        if (cameraid > num_cameras) {
            ALOGE("camera service provided cameraid out of bounds, "
                    "cameraid = %d, num supported = %d",
                    cameraid, num_cameras);
            rv = -EINVAL;
            goto fail;
        }

        camera_device = (wrapper_camera_device_t*)malloc(sizeof(*camera_device));
        if (!camera_device) {
            ALOGE("camera_device allocation fail");
            rv = -ENOMEM;
            goto fail;
        }
        memset(camera_device, 0, sizeof(*camera_device));
        camera_device->id = cameraid;

        rv = gVendorModule->common.methods->open(
                (const hw_module_t*)gVendorModule, name,
                (hw_device_t**)&(camera_device->vendor));
        if (rv) {
            ALOGE("vendor camera open fail");
            goto fail;
        }
        ALOGV("%s: got vendor camera device 0x%08X",
                __FUNCTION__, (uintptr_t)(camera_device->vendor));

        camera_ops = (camera_device_ops_t*)malloc(sizeof(*camera_ops));
        if (!camera_ops) {
            ALOGE("camera_ops allocation fail");
            rv = -ENOMEM;
            goto fail;
        }

        memset(camera_ops, 0, sizeof(*camera_ops));

        camera_device->base.common.tag = HARDWARE_DEVICE_TAG;
        camera_device->base.common.version = CAMERA_DEVICE_API_VERSION_1_0;
        camera_device->base.common.module = (hw_module_t *)(module);
        camera_device->base.common.close = camera_device_close;
        camera_device->base.ops = camera_ops;

        camera_ops->set_preview_window = camera_set_preview_window;
        camera_ops->set_callbacks = camera_set_callbacks;
        camera_ops->enable_msg_type = camera_enable_msg_type;
        camera_ops->disable_msg_type = camera_disable_msg_type;
        camera_ops->msg_type_enabled = camera_msg_type_enabled;
        camera_ops->start_preview = camera_start_preview;
        camera_ops->stop_preview = camera_stop_preview;
        camera_ops->preview_enabled = camera_preview_enabled;
        camera_ops->store_meta_data_in_buffers = camera_store_meta_data_in_buffers;
        camera_ops->start_recording = camera_start_recording;
        camera_ops->stop_recording = camera_stop_recording;
        camera_ops->recording_enabled = camera_recording_enabled;
        camera_ops->release_recording_frame = camera_release_recording_frame;
        camera_ops->auto_focus = camera_auto_focus;
        camera_ops->cancel_auto_focus = camera_cancel_auto_focus;
        camera_ops->take_picture = camera_take_picture;
        camera_ops->cancel_picture = camera_cancel_picture;
        camera_ops->set_parameters = camera_set_parameters;
        camera_ops->get_parameters = camera_get_parameters;
        camera_ops->put_parameters = camera_put_parameters;
        camera_ops->send_command = camera_send_command;
        camera_ops->release = camera_release;
        camera_ops->dump = camera_dump;

        *device = &camera_device->base.common;
    }

    return rv;

fail:
    if (camera_device) {
        free(camera_device);
        camera_device = NULL;
    }
    if (camera_ops) {
        free(camera_ops);
        camera_ops = NULL;
    }
    *device = NULL;
    return rv;
}

static int camera_get_number_of_cameras(void)
{
    ALOGV("%s", __FUNCTION__);
    if (check_vendor_module())
        return 0;
    return gVendorModule->get_number_of_cameras();
}

static int camera_get_camera_info(int camera_id, struct camera_info *info)
{
    ALOGV("%s", __FUNCTION__);
    if (check_vendor_module())
        return 0;
    return gVendorModule->get_camera_info(camera_id, info);
}
