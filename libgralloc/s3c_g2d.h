/* g2d/s3c_g2d.h
 *
 * Copyright (c) 2010 Tomasz Figa <tomasz.figa@gmail.com>
 *
 * Samsung S3C G2D driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _S3C_G2D_H_
#define _S3C_G2D_H_

#include <linux/ioctl.h>

#define G2D_IOCTL_MAGIC			'G'

#define S3C_G2D_ROTATOR_0		_IO(G2D_IOCTL_MAGIC,0, struct s5p_g2d_params)
#define S3C_G2D_ROTATOR_90		_IO(G2D_IOCTL_MAGIC,1, struct s5p_g2d_params)
#define S3C_G2D_ROTATOR_180		_IO(G2D_IOCTL_MAGIC,2, struct s5p_g2d_params)
#define S3C_G2D_ROTATOR_270		_IO(G2D_IOCTL_MAGIC,3, struct s5p_g2d_params)
#define S3C_G2D_ROTATOR_X_FLIP		_IO(G2D_IOCTL_MAGIC,4, struct s5p_g2d_params)
#define S3C_G2D_ROTATOR_Y_FLIP		_IO(G2D_IOCTL_MAGIC,5, struct s5p_g2d_params)

/*
 * S3C_G2D_BITBLT
 * Start hardware bitblt operation.
 * Argument:	a pointer to struct s3c_g2d_req with operation parameters
 * Returns:	  0 on success,
 *		< 0, on error
 */
#define S3C_G2D_BITBLT			_IOW(G2D_IOCTL_MAGIC, 6, struct s3c_g2d_params)

/*
 * S3C_G2D_FILLRECT
 * Start hardware fillrect operation.
 * Argument:	a pointer to struct s3c_g2d_fillrect with operation parameters
 * Returns:	  0 on success,
 *		< 0, on error
 */
#define S3C_G2D_FILLRECT		_IOW(G2D_IOCTL_MAGIC, 7, struct s3c_g2d_fillrect)


/*
 * S3C_G2D_SET_ALPHA_VAL
 * Set requested plane alpha value.
 * Argument:	a value from <0, ALPHA_VALUE_MAX> range
 */
#define S3C_G2D_SET_ALPHA_VAL		_IO(G2D_IOCTL_MAGIC, 8)
#define ALPHA_VALUE_MAX			255

/*
 * S3C_G2D_SET_RASTER_OP
 * Set requested raster operation. 
 * Argument:	an 8-bit value defining the operation
 */
#define S3C_G2D_SET_RASTER_OP		_IO(G2D_IOCTL_MAGIC, 9)
#define G2D_ROP_SRC_ONLY		(0xCC)
#define G2D_ROP_3RD_OPRND_ONLY		(0xF0)
#define G2D_ROP_DST_ONLY		(0xAA)
#define G2D_ROP_SRC_OR_DST		(0xEE)
#define G2D_ROP_SRC_OR_3RD_OPRND	(0xFC)
#define G2D_ROP_SRC_AND_DST		(0x88) 
#define G2D_ROP_SRC_AND_3RD_OPRND	(0xC0)
#define G2D_ROP_SRC_XOR_3RD_OPRND	(0x3C)
#define G2D_ROP_DST_OR_3RD_OPRND	(0xFA)

/*
 * S3C_G2D_SET_BLENDING
 * Set requested alpha blending mode.
 * Argument:	one of G2D_*_ALPHA values
 */
#define S3C_G2D_SET_BLENDING		_IO(G2D_IOCTL_MAGIC, 10)
enum
{
	G2D_NO_ALPHA = 0,
	G2D_PLANE_ALPHA,
	G2D_PIXEL_ALPHA // with fallback to plane alpha
};

/* Maximum values for the hardware */
#define G2D_MAX_WIDTH				(8000)
#define G2D_MAX_HEIGHT				(8000)

/* Image data */
struct s3c_g2d_image
{
	uint32_t	base;	// image base address (NULL to use fd)
	int	        fd;	// image file descriptor (for PMEM)
	uint32_t	offs;	// buffer offset
	uint32_t	w;	// image full width
	uint32_t	h;	// image full height
	uint32_t	l;	// x coordinate of left edge
	uint32_t	t;	// y coordinate of top edge
	uint32_t	r;	// x coordinage of right edge
	uint32_t	b;	// y coordinate of bottom edge
	uint32_t	fmt;	// color format
};

typedef enum
{
	G2D_RGBA_8888 = 1,
	G2D_RGBX_8888 = 2,
	G2D_ARGB_8888 = 3,
	G2D_XRGB_8888 = 4,
	G2D_BGRA_8888 = 5,
	G2D_BGRX_8888 = 6,
	G2D_ABGR_8888 = 7,
	G2D_XBGR_8888 = 8,
	G2D_RGB_888   = 9,
	G2D_BGR_888   = 10,
	G2D_RGB_565   = 11,
	G2D_BGR_565   = 12,
	G2D_RGBA_5551 = 13,
	G2D_ARGB_5551 = 14,
	G2D_RGBA_4444 = 15,
	G2D_ARGB_4444 = 16
} G2D_COLOR_FMT;

/* Bitblt request */
struct s3c_g2d_req
{
	struct s3c_g2d_image src; // source image
	struct s3c_g2d_image dst; // destination image
};

/* Fillrect request */
struct s3c_g2d_fillrect
{
	struct s3c_g2d_image dst;
	uint32_t color;
	uint8_t alpha;
};

typedef struct
{
	uint32_t	src_base_addr;          //Base address of the source image
	uint32_t	src_full_width;         //source image full width
	uint32_t	src_full_height;        //source image full height
	uint32_t	src_start_x;            //coordinate start x of source image
	uint32_t	src_start_y;            //coordinate start y of source image
	uint32_t	src_work_width;         //source image width for work
	uint32_t src_work_height;        //source image height for work
	G2D_COLOR_FMT src_colorfmt; //source color format
	uint32_t src_select;             //source selection

	uint32_t	dst_base_addr;          //Base address of the destination image	
	uint32_t	dst_full_width;         //destination screen full width
	uint32_t	dst_full_height;        //destination screen full width
	uint32_t	dst_start_x;            //coordinate start x of destination screen
	uint32_t	dst_start_y;            //coordinate start y of destination screen
	uint32_t	dst_work_width;         //destination screen width for work
	uint32_t dst_work_height;        //destination screen height for work
	G2D_COLOR_FMT dst_colorfmt; //destination color format
	uint32_t dst_select;             //destination selection

	// Coordinate (X, Y) of clipping window
	uint32_t  cw_x1, cw_y1;
	uint32_t  cw_x2, cw_y2;

	uint32_t  color_val[8];
	uint32_t FGColor;                //Foreground Color
	uint32_t BGColor;                //Background Color
	uint32_t BSCOlor;                //BlueScreen Color
	//ROP 
	uint32_t Masked_select;          //THIRD_OPERAND
	uint32_t UnMasked_select;
	uint32_t MaskedROP3;             //Raster Operation Value
	uint32_t UnMaskedROP3;
	uint32_t transparent_mode;

	uint32_t	alpha_mode;             //true : enable, false : disable
	uint32_t	alpha_val;
	uint32_t fading_offset;
	uint32_t	color_key_mode;         //true : enable, false : disable
	uint32_t	color_key_val;          //transparent color value

}s5p_g2d_params;

#endif
