/*
 *	$Id: ctxt_conv.c,v 1.2 2000-07-12 16:51:56 haley Exp $
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
 *      File:		ctxt_conv.c
 *
 *      Author:		John Clyne
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *      Date:		Mon May  6 14:51:38 MDT 1991
 *
 *      Description:	These are the ctxt conversion routines which convert
 *			raw data to its device dependent format.
 *			These routines assume sufficient space exists in the
 *			destination list. 
 */

#include <stdio.h>
#include "gksc.h"
#include "ctxt.h"
#include "ctxt_device.h"

/*ARGSUSED*/
void	ctxt_ConvPoints(ddp, rawx, rawy, points, n, conv)
	GKSC_Ptr	ddp;
	float	*rawx, *rawy;
	Points	*points;
	int	*n;
	int	conv;
{
	CTXTPoint	*point_ptr = (CTXTPoint *) points->list; 
	int	index = points->num;
	
	int	i;

	if (conv == RAW_TO_COOKED) {
		for (i=0; i<*n; index++, i++) {
			point_ptr[index].x = rawx[i];
			point_ptr[index].y = rawy[i];
		}
		points->num = index;
	}
	else {
		for (i=0; i<*n; i++) {
			rawx[i] = point_ptr[i].x;
			rawy[i] = point_ptr[i].y;
		}
	}
}

/*ARGSUSED*/
void	ctxt_ConvString(ddp, raw, string, n, conv)
	GKSC_Ptr	ddp;
	int	*raw;
	String	*string;
	int	*n;
	int	conv;
{
	char	*s = (char *) string->list;

	int	index = string->num;

	int	i;

	if (conv == RAW_TO_COOKED) {
		for (i=0; i<*n; i++, index++) {
			s[index] = raw[i];
		}

		s[index] = '\0';
		string->num = index;
	}
	else {
		for (i=0; i<*n; i++) {
			raw[i] = s[i];
		}
	}
}

/*ARGSUSED*/
void	ctxt_ConvInts(ddp, raw, ints, n, conv)
	GKSC_Ptr	ddp;
	int	*raw;
	Ints	*ints;
	int	*n;
	int	conv;
{
	int	*iptr = (int *) ints->list;
	int	index = ints->num;

	int	i;

	if (conv == RAW_TO_COOKED) {
		for (i=0; i<*n; i++, index++) {
			iptr[index] = raw[i];
		}

		ints->num = index;
	}
	else {
		for (i=0; i<*n; i++) {
			raw[i] = iptr[i];
		}
	}
}

/*ARGSUSED*/
void	ctxt_ConvFloats(ddp, raw, floats, n, conv)
	GKSC_Ptr	ddp;
	float	*raw;
	Floats	*floats;
	int	*n;
	int	conv;
{
	float	*fptr = (float *) floats->list;
	int	index = floats->num;

	int	i;

	if (conv == RAW_TO_COOKED) {
		for (i=0; i<*n; index++, i++) {
			fptr[index] = raw[i];
		}

		floats->num = index;
	}
	else {
		for (i=0; i<*n; i++) {
			raw[i] = fptr[i];
		}
	}
}

/*ARGSUSED*/
void	ctxt_ConvIndexes(ddp, raw, indexes, n, conv)
	GKSC_Ptr	ddp;
	int	*raw;
	Indexes	*indexes;
	int	*n;
	int	conv;
{
	int	*ind = (int *) indexes->list;
	int	index = indexes->num;

	int	i;

	if (conv == RAW_TO_COOKED) {
		for (i=0; i<*n; index++, i++) {
			ind[index] = raw[i];
		}

		indexes->num = index;
	}
	else {
		for (i=0; i<*n; i++) {
			raw[i] = ind[i];
		}
	}
}

/*ARGSUSED*/
void	ctxt_ConvRGBs(ddp, raw, rgbs, n, conv)
	GKSC_Ptr	ddp;
	float	*raw;
	RGBs	*rgbs;
	int	*n;
	int	conv;
{
	CTXTColor	*color = (CTXTColor *) rgbs->list;
	int		index = rgbs->num;

	int	i;

	if (conv == RAW_TO_COOKED) {
		for (i=0; i<*n; i +=3, index++) {
			color[index].r = raw[i];
			color[index].g = raw[i+1];
			color[index].b = raw[i+2];
		}

		rgbs->num = index;
	}
	else {
		for (index=0, i=0; i<*n; i +=3, index++) {
			raw[i] = color[index].r;
			raw[i+1] = color[index].g;
			raw[i+2] = color[index].b;
		}
		/*
		 * on exit, *n is the number of elements written into
		 * the raw array. 
		 */
		*n = i;
	}
}
