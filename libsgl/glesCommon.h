/*
 * libsgl/glesCommon.h
 *
 * SAMSUNG S3C6410 FIMG-3DSE (PROPER) OPENGL ES IMPLEMENTATION
 *
 * Copyrights:	2010 by Tomasz Figa < tomasz.figa at gmail.com >
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _GLESCOMMON_H_
#define _GLESCOMMON_H_

#include <cassert>

#include "platform.h"
#include "state.h"
#include "types.h"
#include "fglpixelformat.h"

static inline GLint unitFromTextureEnum(GLenum texture)
{
	GLint unit;

	switch(texture) {
	case GL_TEXTURE0:
		unit = 0;
		break;
	case GL_TEXTURE1:
		unit = 1;
		break;
#if 0
	case TEXTURE2:
		unit = 2;
		break;
	case TEXTURE3:
		unit = 3;
		break;
	case TEXTURE4:
		unit = 4;
		break;
	case TEXTURE5:
		unit = 5;
		break;
	case TEXTURE6:
		unit = 6;
		break;
	case TEXTURE7:
		unit = 7;
		break;
#endif
	default:
		return -1;
	}

	return unit;
}

/*
	Context management
*/

#ifdef GLES_DEBUG
#define getContext() ( \
	ALOGD("%s called getContext()", __func__), \
	_getContext())
static inline FGLContext *_getContext(void)
#else
static inline FGLContext *getContext(void)
#endif
{
	FGLContext *ctx = getGlThreadSpecific();

	if(!ctx) {
		ALOGE("GL context is NULL!");
		exit(EINVAL);
	}

	return ctx;
}

/*
	Error handling
*/

extern pthread_mutex_t glErrorKeyMutex;
extern pthread_key_t glErrorKey;

#ifdef GLES_ERR_DEBUG
#define setError(a) ( \
	ALOGD("GLES error %s in %s", #a, __func__), \
	_setError(a))
static inline void _setError(GLenum error)
#else
static inline void setError(GLenum error)
#endif
{
	GLenum errorCode;

	if(unlikely(glErrorKey == (pthread_key_t)-1)) {
		pthread_mutex_lock(&glErrorKeyMutex);
		if(glErrorKey == (pthread_key_t)-1)
			pthread_key_create(&glErrorKey, NULL);
		pthread_mutex_unlock(&glErrorKeyMutex);
		errorCode = GL_NO_ERROR;
	} else {
		errorCode = (GLenum)pthread_getspecific(glErrorKey);
	}

	pthread_setspecific(glErrorKey, (void *)error);

	if(errorCode == GL_NO_ERROR)
		errorCode = error;
}

#endif
