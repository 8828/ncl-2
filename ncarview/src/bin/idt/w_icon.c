/*
 *	$Id: w_icon.c,v 1.6 2000-08-22 15:11:10 haley Exp $
 */
/************************************************************************
*                                                                       *
*                Copyright (C)  2000                                    *
*        University Corporation for Atmospheric Research                *
*                All Rights Reserved                                    *
*                                                                       *
* This file is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published     *
* by the Free Software Foundation; either version 2 of the License, or  *
* (at your option) any later version.                                   *
*                                                                       *
* This software is distributed in the hope that it will be useful, but  *
* WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
* General Public License for more details.                              *
*                                                                       *
* You should have received a copy of the GNU General Public License     *
* along with this software; if not, write to the Free Software         *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307   *
* USA.                                                                  *
*                                                                       *
************************************************************************/

/*
 *	w_icon.c
 *
 *	Author		John Clyne
 *
 *	Date		Tue Jul 23 16:28:25 MDT 1991
 *
 *	Create a icon pixmap and install it on the toplevel widget if the
 *	user hasn't already requested a icon pixmap. We imbed the code for
 *	the bitmap so we can avoid having to install bitmaps non standard
 *	bitmap directory
 */
#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>


/*
 *	bitmap for default icon
 */
#include "earth.xbm"
static  struct  {
	int     width,
	height;
	unsigned char    *bits;
	Pixmap  pmap;
	} icon = {earth_width, earth_height, earth_bits, 0};

SetIconResource(toplevel)
	Widget	toplevel;
{

	Arg		args[5];
	Cardinal	n;
	/*
	 * Get icon pixmap if its been set.
	 */
	n = 0;
	XtSetArg(args[n], XtNiconPixmap, &icon.pmap);   n++;
	XtGetValues(toplevel, args, n);

	/*
	 * don't know if this is portable but there doesn't seem to be any
	 * other way
	 */
	if (icon.pmap == 0) {
		/*
		 * user hasn't specified a icon pixmap resource so use
		 * the default
	 	 */
		icon.pmap = XCreateBitmapFromData(
			XtDisplay(toplevel),
			RootWindowOfScreen(XtScreen(toplevel)),
			(char *) icon.bits, icon.width, icon.height
		);

		n = 0;
		XtSetArg(args[n], XtNiconPixmap, icon.pmap);    n++;
		XtSetValues(toplevel, args, n);
	}


}
