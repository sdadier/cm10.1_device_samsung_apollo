/*
* Copyright (C) 2008 The Android Open Source Project
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

#define LOG_TAG "copybit"

#include <cutils/log.h>

#include <linux/fb.h>

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>

#include "copybit.h"
#include <hardware/hardware.h>

#include <linux/android_pmem.h>

#include "gralloc_priv.h"
#include "s3c_g2d.h"

#define MAX_DIMENSION 2040

#ifdef COPYBIT_DEBUG
#define DBOUT(arg,...) \
ALOGI(arg,__VA_ARGS__)
#endif

#define DEBUG_G2D_ERRORS 0


/** State information for each device instance */

struct copybit_context_t {
    struct copybit_device_t device;

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

/**
* Common hardware methods
*/

static int open_copybit(const struct hw_module_t* module, const char* name,
        struct hw_device_t** device);

static struct hw_module_methods_t copybit_module_methods = {
    open: open_copybit
};

/*
* The COPYBIT Module
*/
struct copybit_module_t HAL_MODULE_INFO_SYM = {
    common: {
        tag: HARDWARE_MODULE_TAG,
        version_major: 1,
        version_minor: 0,
        id: COPYBIT_HARDWARE_MODULE_ID,
        name: "S5P6442 COPYBIT Module",
        author: "Sebastien Dadier",
        methods: &copybit_module_methods
    }
};

/** min of int a, b */
static inline int min(int a, int b) {
    return (a<b) ? a : b;
}

/** max of int a, b */
static inline int max(int a, int b) {
    return (a>b) ? a : b;
}

/** scale each parameter by mul/div. Assume div isn't 0 */
static inline void MULDIV(uint32_t *a, uint32_t *b, int mul, int div) {
    if (mul != div) {
        *a = (mul * *a) / div;
        *b = (mul * *b) / div;
    }
}

/** Determine the intersection of lhs & rhs store in out */
static void intersect(struct copybit_rect_t *out,
                      const struct copybit_rect_t *lhs,
                      const struct copybit_rect_t *rhs) {
    out->l = max(lhs->l, rhs->l);
    out->t = max(lhs->t, rhs->t);
    out->r = min(lhs->r, rhs->r);
    out->b = min(lhs->b, rhs->b);
}

/** convert COPYBIT_FORMAT to G2D format */
static int get_format(int format) {

	switch (format) 
	{
		case COPYBIT_FORMAT_RGB_565:       return G2D_RGB_565;
		case COPYBIT_FORMAT_RGBX_8888:     return G2D_RGBX_8888;
		case COPYBIT_FORMAT_RGB_888:       return G2D_RGB_888;
		case COPYBIT_FORMAT_RGBA_8888:     return G2D_RGBA_8888;
		case COPYBIT_FORMAT_BGRA_8888:     return G2D_ARGB_8888;
		//case COPYBIT_FORMAT_YCbCr_422_SP: return -1;
		//case COPYBIT_FORMAT_YCbCr_420_SP: return G2D_RGB_565;
		//case COPYBIT_FORMAT_YCrCb_420_SP: return G2D_RGB_565;
		default :
			ALOGE("Copybit HAL unsupport format ! ,format is 0x%x",format);
			return -1;
	}

}

static uint32_t get_rotate(int transform)
{
int g2d_rotate = S3C_G2D_ROTATOR_0;

switch(transform)
{
   /* flip source image horizontally */
   case COPYBIT_TRANSFORM_FLIP_H:
    g2d_rotate = S3C_G2D_ROTATOR_X_FLIP;
    break;
   
   /* flip source image vertically */
   case COPYBIT_TRANSFORM_FLIP_V:
    g2d_rotate = S3C_G2D_ROTATOR_Y_FLIP;
    break;

   /* rotate source image 90 degres */
   case COPYBIT_TRANSFORM_ROT_90:
    g2d_rotate = S3C_G2D_ROTATOR_90;
    break;
  
   /* rotate source image 180 degres */
   case COPYBIT_TRANSFORM_ROT_180:
    g2d_rotate = S3C_G2D_ROTATOR_180;
    break;

   /* rotate source image 270 degres */
   case COPYBIT_TRANSFORM_ROT_270:
    g2d_rotate = S3C_G2D_ROTATOR_270;
    break;
}
return g2d_rotate;
}

/** convert from copybit image to s3c_g2d_params structure */
static int set_image(struct copybit_context_t *dev, s3c_g2d_params *req, const struct copybit_image_t *rhs, int is_src) 
{
    int status = 0;
    private_handle_t* hnd = (private_handle_t*)rhs->handle;

    if (is_src) {
        req->src_full_width  = rhs->w;
        req->src_full_height = rhs->h;
        req->src_work_width  = rhs->w - 1;
        req->src_work_height = rhs->h - 1;
	req->src_colorfmt = get_format(rhs->format);
    } else {
        req->dst_full_width  = rhs->w;
        req->dst_full_height = rhs->h;
        req->dst_work_width  = rhs->w - 1;
        req->dst_work_height = rhs->h - 1;
	req->dst_colorfmt = get_format(rhs->format);
    }
	

    if (hnd->flags & private_handle_t::PRIV_FLAGS_FRAMEBUFFER) {
	//LOGI("set_image : is_src=%d - flag PRIV_FLAGS_FRAMEBUFFER", is_src);
        if (is_src)
            req->src_base_addr = hnd->smem_start + hnd->offset;
        else
            req->dst_base_addr = hnd->smem_start + hnd->offset;
    } else if (hnd->flags & private_handle_t::PRIV_FLAGS_USES_PMEM) {
        struct pmem_region region;
	
	//LOGI("set_image : is_src=%d - flag PRIV_FLAGS_USES_PMEM", is_src);
        memset(&region, 0, sizeof(struct pmem_region));
        if (ioctl(hnd->fd, PMEM_GET_PHYS, &region) < 0)
        {
            status = -errno;
            ALOGE("set_image: PMEM_GET_PHYS failed!(%s)\n", strerror(status));
            goto error;
        }
	
        if (is_src)
            req->src_base_addr = region.offset + hnd->offset;
       else
            req->dst_base_addr = region.offset + hnd->offset;
        
    } else {
        ALOGE("set_image: unsupport flags 0x%08x\n", hnd->flags);
        status = -EINVAL;
    }

error:
    return status;
	
}

/** setup rectangles */
static void set_rects(struct copybit_context_t *dev,
                      s3c_g2d_params *req,
                      const struct copybit_rect_t *dst,
                      const struct copybit_rect_t *src,
                      const struct copybit_rect_t *scissor) {
#if 0
    struct copybit_rect_t clip;
    intersect(&clip, scissor, dst);

    e->dst_rect.x  = clip.l;
    e->dst_rect.y  = clip.t;
    e->dst_rect.w  = clip.r - clip.l;
    e->dst_rect.h  = clip.b - clip.t;

    uint32_t W, H;
    if (dev->mFlags & COPYBIT_TRANSFORM_ROT_90) {
        e->src_rect.x = (clip.t - dst->t) + src->t;
        e->src_rect.y = (dst->r - clip.r) + src->l;
        e->src_rect.w = (clip.b - clip.t);
        e->src_rect.h = (clip.r - clip.l);
        W = dst->b - dst->t;
        H = dst->r - dst->l;
    } else {
        e->src_rect.x  = (clip.l - dst->l) + src->l;
        e->src_rect.y  = (clip.t - dst->t) + src->t;
        e->src_rect.w  = (clip.r - clip.l);
        e->src_rect.h  = (clip.b - clip.t);
        W = dst->r - dst->l;
        H = dst->b - dst->t;
    }
    MULDIV(&e->src_rect.x, &e->src_rect.w, src->r - src->l, W);
    MULDIV(&e->src_rect.y, &e->src_rect.h, src->b - src->t, H);
    if (dev->mFlags & COPYBIT_TRANSFORM_FLIP_V) {
        e->src_rect.y = e->src.height - (e->src_rect.y + e->src_rect.h);
    }
    if (dev->mFlags & COPYBIT_TRANSFORM_FLIP_H) {
        e->src_rect.x = e->src.width  - (e->src_rect.x + e->src_rect.w);
    }
#endif
    req->src_start_x = src->l;
    req->src_start_y = src->t;
    req->dst_start_x = dst->l;
    req->dst_start_y = dst->t;

    req->cw_x1 = 0;
    req->cw_y1 = 0;
 //   req->cw_x2 = dev->vinfo.xres;
 //   req->cw_y2 = dev->vinfo.yres;
 req->cw_x2 = req->dst_start_x+req->dst_work_width;
req->cw_y2 = req->dst_start_y+req->dst_work_height;
}



/** copy the bits */
static int s3c64xx_copybit(struct copybit_context_t *dev, s3c_g2d_params const *req)
{
    int err;
    unsigned int cmd = S3C_G2D_ROTATOR_0;

switch(dev->transform)
{
   case 0:
    cmd = S3C_G2D_ROTATOR_0;

    break;
   case 4:
    cmd = S3C_G2D_ROTATOR_90;  
    break;
   case 3:
    cmd = S3C_G2D_ROTATOR_180;
    break;
   case 7:
    cmd = S3C_G2D_ROTATOR_270;
    break;
   default:
    cmd = S3C_G2D_ROTATOR_0;
    break;   
}

/*    LOGE("s3c64xx_copybit: cmd = 0x%08x\n", cmd);
    LOGE("src_base_addr 0x%08x dst_base_addr 0x%08x\n", req->src_base_addr, req->dst_base_addr);
    LOGE("src_start_x %d src_start_y %d src_full_width %d src_full_height %d\n", req->src_start_x, req->src_start_y, req->src_full_width, req->src_full_height);
    LOGE("dst_start_x %d dst_start_y %d dst_full_width %d dst_full_height %d\n", req->dst_start_x, req->dst_start_y, req->dst_full_width, req->dst_full_height);
    LOGE("cw_x1 %d cw_y1 %d cw_x2 %d cw_y2 %d\n", req->cw_x1, req->cw_y1, req->cw_x2, req->cw_y2);
    for(int i = 0; i < 8; i++)
        LOGE("color_val[%d] 0x%08x\n", i, req->color_val[i]);

    LOGE("alpha_mode %d alpha_val %d\n", req->alpha_mode, req->alpha_val);
    LOGE("color_key_mode %d color_key_val %d\n", req->color_key_mode, req->color_key_val);*/
    err = ioctl(dev->mFD_G2D, cmd, req);
    ALOGE_IF(err<0, "copyBits failed (%s)", strerror(errno));
    if (err == 0) {
        return 0;
    } else {
        return -errno;
    }
}


/*****************************************************************************/

/** Set a parameter to value */
static int set_parameter_copybit(struct copybit_device_t *dev, int name, int value) 
{
    struct copybit_context_t* ctx = (struct copybit_context_t*)dev;
int status=0;

   switch (name) 
{
    case COPYBIT_ROTATION_DEG:
       //LOGI("in copybit set_parameter_copybit: name=%d value=%d\n",name,value);
    ctx->rotation = value;
    if(!((value==0)||(value==90)||(value==270)))
    {
     ALOGE("In valid rotation degree\n");   
     status=-EINVAL;        
    }
        break;

    case COPYBIT_PLANE_ALPHA:
    if(value<0) value=0;
    if(value>255) value=255;
        ctx->alpha = value;
        break;

    case COPYBIT_DITHER:
        ctx->dither = value;
        break;

    case COPYBIT_TRANSFORM:
        ctx->transform = value;
        break;

    default:
       ALOGE("name of the set parameter not implemented\n");
        return -EINVAL;
    }
    return status;

}

/** Get a static info value */
static int get(struct copybit_device_t *dev, int name) 
{
    struct copybit_context_t* ctx = (struct copybit_context_t*)dev;
    int value;
//LOGI("in copybit get: name=%d\n",name);
    if (ctx) {
        switch(name) {
        case COPYBIT_MINIFICATION_LIMIT:
            value = 4;
            break;
        case COPYBIT_MAGNIFICATION_LIMIT:
            value = 4;
            break;
        case COPYBIT_SCALING_FRAC_BITS:
            value = 32;
            break;
        case COPYBIT_ROTATION_STEP_DEG:
            value = 90;
            break;
        default:
            value = -EINVAL;
        }
    } else {
        value = -EINVAL;
    }
    return value;
}

static int stretch_copybit(
        struct copybit_device_t *dev,
        struct copybit_image_t const *dst,
        struct copybit_image_t const *src,
        struct copybit_rect_t const *dst_rect,
        struct copybit_rect_t const *src_rect,
        struct copybit_region_t const *region) 
{
    struct copybit_context_t* ctx = (struct copybit_context_t*)dev;
    int status = 0;
/*    LOGE("stretch_copybit: +++\n");
    LOGE("src->w %d src->h %d dst->w %d dst->h %d\n", src->w, src->h, dst->w, dst->h);
    LOGE("src_rect:(%d,%d) (%d, %d)\n", src_rect->l, src_rect->t, src_rect->r, src_rect->b);
    LOGE("dst_rect:(%d,%d) (%d, %d)\n", dst_rect->l, dst_rect->t, dst_rect->r, dst_rect->b);
    LOGE("src->format = %d\n", src->format);*/
    if (ctx) {
        if (ctx->alpha < 255) {
            switch (src->format) {
                // we don't support plane alpha with RGBA formats
                //case COPYBIT_FORMAT_RGBA_8888:
                case COPYBIT_FORMAT_BGRA_8888:
                //case COPYBIT_FORMAT_RGBA_5551:
                case COPYBIT_FORMAT_RGBA_4444:
                    return -EINVAL;
            }
        }

        if (src_rect->l < 0 || src_rect->r > src->w ||
            src_rect->t < 0 || src_rect->b > src->h) {
            // this is always invalid
            return -EINVAL;
        }

        if (src->w > MAX_DIMENSION || src->h > MAX_DIMENSION)
            return -EINVAL;

        if (dst->w > MAX_DIMENSION || dst->h > MAX_DIMENSION)
            return -EINVAL;

        const struct copybit_rect_t bounds = { 0, 0, dst->w, dst->h };
        struct copybit_rect_t clip;
        status = 0;
        while ((status == 0) && region->next(region, &clip)) {
            intersect(&clip, &bounds, &clip);
            s3c_g2d_params req;
            memset(&req, 0, sizeof(s3c_g2d_params));

            //set_infos(ctx, &req);

            status = set_image(ctx, &req, dst, 0);
            if (status < 0)
                break;

            status = set_image(ctx, &req, src, 1);
            if (status < 0)
                break;

            set_rects(ctx, &req, dst_rect, src_rect, &clip);
//req.dst_select 	    = 0;
//req.src_select 	    = 1;
            status = s3c64xx_copybit(ctx, &req);
        }

    } else {
        status = -EINVAL;
    }
//    LOGE("stretch_copybit: --- status = %d\n", status);
    return status;
}

/** Perform a blit type operation */
static int blit_copybit(
        struct copybit_device_t *dev,
        struct copybit_image_t const *dst,
        struct copybit_image_t const *src,
        struct copybit_region_t const *region) 
{
    struct copybit_rect_t dr = { 0, 0, dst->w, dst->h };
    struct copybit_rect_t sr = { 0, 0, src->w, src->h };
    return stretch_copybit(dev, dst, src, &dr, &sr, region);
}

/*****************************************************************************/
#if 0
/** Close the copybit device */
static int close_copybit(struct hw_device_t *dev) 
{
    struct copybit_context_t* ctx = (struct copybit_context_t*)dev;
    if (ctx) {
        close(ctx->mFD_G2D);
        free(ctx);
    }
    return 0;
}

/** Open a new instance of a copybit device using name */
static int open_copybit(const struct hw_module_t* module, const char* name,
        struct hw_device_t** device)
{
    int status = -EINVAL;

    copybit_context_t *ctx;
    ctx = (copybit_context_t *)malloc(sizeof(copybit_context_t));
    memset(ctx, 0, sizeof(*ctx));

    ctx->device.common.tag = HARDWARE_DEVICE_TAG;
    ctx->device.common.version = 1;
    ctx->device.common.module = const_cast<hw_module_t*>(module);
    ctx->device.common.close = close_copybit;
    ctx->device.set_parameter = set_parameter_copybit;
    ctx->device.get = get;
    ctx->device.blit = blit_copybit;
    ctx->device.stretch = stretch_copybit;

ctx->alpha = 0xff;
ctx->rotation = 0;
ctx->transform = 0;

    ctx->mFD_G2D = open("/dev/s3c-g2d", 0);
    
    if (ctx->mFD_G2D < 0) 
{
        status = errno;
        ALOGE("Error opening frame buffer errno=%d (%s)", status, strerror(status));
        status = -status;
   }

    *device = &ctx->device.common;
status = 0;

    return status;
}

#endif
/** Close the copybit device */
static int close_copybit(struct hw_device_t *dev) 
{
    struct copybit_context_t* ctx = (struct copybit_context_t*)dev;
    if (ctx) {
        close(ctx->mFD_G2D);
        close(ctx->mFD);
        free(ctx);
    }
    return 0;
}

/** Open a new instance of a copybit device using name */
static int open_copybit(const struct hw_module_t* module, const char* name,
        struct hw_device_t** device)
{
    int status = -EINVAL;
    copybit_context_t *ctx;
    ALOGE("open_copybit: +++\n");
    ctx = (copybit_context_t *)malloc(sizeof(copybit_context_t));
    memset(ctx, 0, sizeof(*ctx));

    ctx->device.common.tag = HARDWARE_DEVICE_TAG;
    ctx->device.common.version = 1;
    ctx->device.common.module = const_cast<hw_module_t*>(module);
    ctx->device.common.close = close_copybit;
    ctx->device.set_parameter = set_parameter_copybit;
    ctx->device.get = get;
    ctx->device.blit = blit_copybit;
    ctx->device.stretch = stretch_copybit;
    ctx->alpha = 0;
    ctx->mFlags = 0;

    ctx->mFD_G2D = open("/dev/s3c-g2d", O_RDWR);
    if (ctx->mFD_G2D <0) {
        status = errno;
        ALOGE("Error opening G2D errno=%d (%s)",
             status, strerror(status));
        status = -status;
        return status;
    }

    ctx->mFD = open("/dev/graphics/fb0", O_RDWR, 0);
    
    if (ctx->mFD < 0) {
        status = errno;
        ALOGE("Error opening frame buffer errno=%d (%s)",
             status, strerror(status));
        status = -status;
    } else {
        struct fb_fix_screeninfo finfo;
        struct fb_var_screeninfo vinfo;
        if (ioctl(ctx->mFD, FBIOGET_FSCREENINFO, &finfo) == 0 && 
            ioctl(ctx->mFD, FBIOGET_VSCREENINFO, &vinfo) == 0) {
#if 0
            if (strcmp(finfo.id, "msmfb") == 0) {
                /* Success */
                status = 0;
            } else {
                ALOGE("Error not msm frame buffer");
                status = -EINVAL;
            }
#endif
            status = 0;
            memcpy(&(ctx->finfo), &finfo, sizeof(struct fb_fix_screeninfo));
            memcpy(&(ctx->vinfo), &vinfo, sizeof(struct fb_var_screeninfo));

        } else {
            ALOGE("Error executing ioctl for screen info");
            status = -errno;
        }
    }

    if (status == 0) {
        *device = &ctx->device.common;
    } else {
        close_copybit(&ctx->device.common);
    }
    ALOGE("open_copybit: ---\n");
    return status;
}
