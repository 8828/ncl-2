/*
 *	$Id: xddi.h,v 1.11 2000-07-12 16:52:02 haley Exp $
 */
/************************************************************************
*                                                                       *
*                Copyright (C)  2000                                    *
*        University Corporation for Atmospheric Research                *
*                All Rights Reserved                                    *
*                                                                       *
* This file is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU Lesser General Public License as        *
* published by the Free Software Foundation; either version 2.1 of the  *
* License, or (at your option) any later version.                       *
*                                                                       *
* This software is distributed in the hope that it will be useful, but  *
* WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
* Lesser General Public License for more details.                       *
*                                                                       *
* You should have received a copy of the GNU Lesser General Public      *
* License along with this software; if not, write to the Free Software  *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307   *
* USA.                                                                  *
*                                                                       *
************************************************************************/

/*
 *      File:		xddi.h
 *
 *      Author:		John Clyne
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *      Date:		Wed May  1 17:49:30 MDT 1991
 *
 *      Description:	This file defines the device dependent structure for
 *			gksc.ddp field for an X11 device
 */

#ifndef	_xddi_
#define	_xddi_

#include <ncarg/gksP.h>
#include "x.h"
#include "gks.h"
#include "common.h"
#include "transform.h"

typedef	struct Xddi_ColorStatus_ {
	int		ref_count;
	unsigned short	red,green,blue;
	Pixeltype	xpixnum;
} XddpColorStatus;

typedef	struct	Xddi_	{
	XWorkType	xwtype;
	int		dead;
	Display		*dpy;
	Screen		*scr;
	Visual		*vis;
	Window		win;
	unsigned int	depth;
	unsigned 	dim;
	Transform2D	transform;
	GC		line_gc,
			marker_gc,
			text_gc,
			fill_gc,
			cell_gc,
			bg_gc,
			hatch_gc;
	int		line_index,
			marker_index,
			text_index,
			fill_index,
			cell_index,
			bg_index;
	Boolean		color_ava;
	XColModel	color_model;
	_NGCXAllocColorProc	alloc_color;
	_NGCXFreeColorsProc	free_colors;
	void		*cref;
	_NGCXGetSizeProc	size_change;
	void			*sref;
	Pixeltype	color_pal[MAX_COLORS];
	int		color_info[MAX_COLORS];
	XddpColorStatus	color_status[MAX_COLORS];
	Boolean		x_ref_count;
	int		max_x_colors;
	int		*color_def;
	Colormap	cmap;
	Boolean		cmap_ro;
	Boolean		mycmap;
	int		marker_type,
			marker_size;
	int		fill_style,
			hatch_index;
	TransSystem	tsystem;
	int		percent_colerr;
	float		pcerr_sqr;
} Xddp;

extern void X11_private_color(
#ifdef	NeedFuncProto
	Xddp	*xi
#endif
);

extern void X11_free_ci(
#ifdef	NeedFuncProto
	Xddp		*xi,
	unsigned	index
#endif
);

#endif	/*	_xddi_	*/
