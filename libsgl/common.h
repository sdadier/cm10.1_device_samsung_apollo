/*
 * libsgl/common.h
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

#ifndef _LIBSGL_COMMON_H_
#define _LIBSGL_COMMON_H_

#define FGL_NPOT_TEXTURES

#define FGL_MAX_TEXTURE_UNITS		2
#define FGL_MAX_TEXTURE_OBJECTS		1024
#define FGL_MAX_BUFFER_OBJECTS		1024
#define FGL_MAX_FRAMEBUFFER_OBJECTS	1024
#define FGL_MAX_RENDERBUFFER_OBJECTS	1024
#define FGL_MAX_MIPMAP_LEVEL		11
#define FGL_MAX_LIGHTS			8
#define FGL_MAX_CLIP_PLANES		1
#define FGL_MAX_MODELVIEW_STACK_DEPTH	16
#define FGL_MAX_PROJECTION_STACK_DEPTH	2
#define FGL_MAX_TEXTURE_STACK_DEPTH	2
#define FGL_MAX_SUBPIXEL_BITS		4
#define FGL_MAX_TEXTURE_SIZE		2047
#define FGL_MAX_VIEWPORT_DIMS		2047
#define FGL_MIN_POINT_SIZE		(1.0f)
#define FGL_MAX_POINT_SIZE		(2048.0f)
#define FGL_MIN_LINE_WIDTH		(1.0f)
#define FGL_MAX_LINE_WIDTH		(128.0f)

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

#define FUNC_UNIMPLEMENTED \
	static int flag = 0; \
	if (!flag) \
		ALOGW("Application called unimplemented function: %s", __func__); \
	flag = 1

//#define TRACE_FUNCTIONS
#ifdef TRACE_FUNCTIONS
class FunctionTracer {
	const char *name;
public:
	inline FunctionTracer(const char *n)
	: name(n)
	{
		ALOGD("Entering %s.", name);
	}

	inline ~FunctionTracer()
	{
		ALOGD("Leaving %s.", name);
	}
};

#define FUNCTION_TRACER	FunctionTracer __ft(__func__)
#else
#define FUNCTION_TRACER
#endif

#undef NELEM
#define NELEM(x) (sizeof(x)/sizeof(*(x)))

#define BIT_MASK(x)	((1UL << (x)) - 1)
#define BIT_VAL(x)	(1UL << (x))

template<typename T>
static inline T max(T a, T b)
{
	if (b > a)
		a = b;
	return a;
}

template<typename T>
static inline T min(T a, T b)
{
	if (b < a)
		a = b;
	return a;
}

template<typename T>
static inline T clamp(T v, T l, T h)
{
	if (v > h)
		v = h;
	if (v < l)
		v = l;
	return v;
}

template<typename T>
static int binarySearch(const T sortedArray[], int first, int last, EGLint key)
{
	while (first <= last) {
		int mid = (first + last) / 2;

		if (key > sortedArray[mid].key)
			first = mid + 1;
		else if (key < sortedArray[mid].key)
			last = mid - 1;
		else
			return mid;
	}

	return -1;
}

#endif // _LIBSGL_COMMON_H_
