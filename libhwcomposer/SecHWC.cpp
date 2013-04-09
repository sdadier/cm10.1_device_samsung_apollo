/*
 * Copyright (C) 2010 The Android Open Source Project
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

#include <hardware/hardware.h>

#include <fcntl.h>
#include <errno.h>

#include <cutils/log.h>
#include <cutils/atomic.h>

#include <hardware/hwcomposer.h>

#include <EGL/egl.h>

#include "s3c_g2d.h"
#include "../libgralloc/gralloc_priv.h"

/*****************************************************************************/

struct hwc_context_t {
    hwc_composer_device_t device;
    /* our private state goes below here */
    int mFD_G2D;
    int mFD;


    int rotation;
    int alpha;
    int dither;
    int transform;
    int mFlags;

    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo vinfo;
};

static int hwc_device_open(const struct hw_module_t* module, const char* name,
        struct hw_device_t** device);

static struct hw_module_methods_t hwc_module_methods = {
    open: hwc_device_open
};

hwc_module_t HAL_MODULE_INFO_SYM = {
    common: {
        tag: HARDWARE_MODULE_TAG,
        version_major: 1,
        version_minor: 0,
        id: HWC_HARDWARE_MODULE_ID,
        name: "Samsung s5p6442 hwcomposer module",
        author: "Sebastien Dadier <s.dadier@gmail.com>",
        methods: &hwc_module_methods,
    }
};

/*****************************************************************************/

static void dump_layer(hwc_layer_t const* l) {
    ALOGD("\ttype=%d, flags=%08x, handle=%p, tr=%02x, blend=%04x, {%d,%d,%d,%d}, {%d,%d,%d,%d}",
            l->compositionType, l->flags, l->handle, l->transform, l->blending,
            l->sourceCrop.left,
            l->sourceCrop.top,
            l->sourceCrop.right,
            l->sourceCrop.bottom,
            l->displayFrame.left,
            l->displayFrame.top,
            l->displayFrame.right,
            l->displayFrame.bottom);
}

static int hwc_prepare(hwc_composer_device_t *dev, hwc_layer_list_t* list) {
    if (list && (list->flags & HWC_GEOMETRY_CHANGED)) {
        for (size_t i=0 ; i<list->numHwLayers ; i++) {
            //dump_layer(&list->hwLayers[i]);
            list->hwLayers[i].compositionType = HWC_FRAMEBUFFER;
        }
    }
    return 0;
}

static int hwc_set(hwc_composer_device_t *dev,
        hwc_display_t dpy,
        hwc_surface_t sur,
        hwc_layer_list_t* list)
{
    //for (size_t i=0 ; i<list->numHwLayers ; i++) {
    //    dump_layer(&list->hwLayers[i]);
    //}

    if (dpy == NULL && sur == NULL && list == NULL) {
        return 0;
    }    
    
    EGLBoolean sucess = eglSwapBuffers((EGLDisplay)dpy, (EGLSurface)sur);
    if (!sucess) {
        return HWC_EGL_ERROR;
    }
    return 0;
}

static int hwc_device_close(struct hw_device_t *dev)
{
    struct hwc_context_t* ctx = (struct hwc_context_t*)dev;
    if (ctx) {
        close(ctx->mFD_G2D);
        close(ctx->mFD);	
        free(ctx);
    }
    return 0;
}

/*****************************************************************************/

static int hwc_device_open(const struct hw_module_t* module, const char* name,
        struct hw_device_t** device)
{
    int status = -EINVAL;
    if (!strcmp(name, HWC_HARDWARE_COMPOSER)) {

        ALOGD("++opening %s", name);
	
        struct hwc_context_t *dev;
        dev = (hwc_context_t*)malloc(sizeof(*dev));

        /* initialize our state here */
        memset(dev, 0, sizeof(*dev));

        /* initialize the procs */
        dev->device.common.tag = HARDWARE_DEVICE_TAG;
        dev->device.common.version = 0;
        dev->device.common.module = const_cast<hw_module_t*>(module);
        dev->device.common.close = hwc_device_close;

        dev->device.prepare = hwc_prepare;
        dev->device.set = hwc_set;

        dev->alpha = 0;
        dev->mFlags = 0;	

	status = 0;
	
	dev->mFD_G2D = open("/dev/s3c-g2d", O_RDWR);
	if (dev->mFD_G2D <0) {
	    status = errno;
	    ALOGE("Error opening G2D errno=%d (%s)",
		status, strerror(status));
	    status = -status;
	    return status;
	}	

	dev->mFD = open("/dev/graphics/fb0", O_RDWR, 0);
	
	if (dev->mFD < 0) {
	    status = errno;
	    ALOGE("Error opening frame buffer errno=%d (%s)",
		status, strerror(status));
	    status = -status;
	} else {
	    struct fb_fix_screeninfo finfo;
	    struct fb_var_screeninfo vinfo;
	    if (ioctl(dev->mFD, FBIOGET_FSCREENINFO, &finfo) == 0 && 
		ioctl(dev->mFD, FBIOGET_VSCREENINFO, &vinfo) == 0) {
	        memcpy(&(dev->finfo), &finfo, sizeof(struct fb_fix_screeninfo));
		memcpy(&(dev->vinfo), &vinfo, sizeof(struct fb_var_screeninfo));
	    } else {
		ALOGE("Error executing ioctl for screen info");
		status = -errno;
	    }
	}

	if (status == 0) {
	    *device = &dev->device.common;
	}	
        
    }
    ALOGE("--open %s",name);
    return status;
}
