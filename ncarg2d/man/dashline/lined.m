.TH LINED 3NCARG "March 1993" UNIX "NCAR GRAPHICS"
.na
.nh
.SH NAME
LINED -
Draws a line segment between two points whose user
coordinates are given.
.SH SYNOPSIS
CALL LINED (XA,XB,YA,YB)
.SH C-BINDING SYNOPSIS
#include <ncarg/ncargC.h>
.sp
void c_lined (float xa, float xb, float ya, float yb)
.SH DESCRIPTION 
.IP XA 12
(an input expression of type REAL) defines the X user coordinate of
the starting point of a line segment.
.IP YA 12
(an input expression of type REAL) defines the Y user coordinate of
the starting point of a line segment.
.IP XB 12
(an input expression of type REAL) defines the X user coordinate of
the ending point of a line segment.
.IP YB 12
(an input expression of type REAL) defines the Y user coordinate of
the ending point of a line segment.
.SH C-BINDING DESCRIPTION
The C-binding argument descriptions are the same as the FORTRAN 
argument descriptions.
.SH EXAMPLES
Use the ncargex command to see the following relevant examples: 
tdashc, tdashl, tdashp, tdashs, 
fcoord1, fcoord2,
fdldashc, fdldashd, fgklnclr, fgklnwth
dashdb, dashdc, frstd, lined, reset, vectd, ncarg_cbind
.SH ACCESS
To use LINED or c_lined, load the NCAR Graphics libraries ncarg, ncarg_gks,
and ncarg_c, preferably in that order.  
.SH SEE ALSO
Online:
dashline, dashline_params, curved,
dashdb, dashdc, frstd, lastd, reset, vectd, ncarg_cbind
.sp
Hardcopy:  
NCAR Graphics Contouring and Mapping Tutorial;
NCAR Graphics Fundamentals, UNIX Version;
User's Guide for NCAR GKS-0A Graphics
.SH COPYRIGHT
Copyright (C) 1987-2003
.br
University Corporation for Atmospheric Research
.br

This documentation is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as published
by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This software is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this software; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
USA.
