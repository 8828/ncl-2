.TH VECTOR 3NCARG "March 1993" UNIX "NCAR GRAPHICS"
.na
.nh
.SH NAME
VECTOR - generates a "pen-down" move to a specified point in the user
coordinate system.  VECTOR is used in conjunction with the routine FRSTPT
to draw lines.
.SH SYNOPSIS
CALL VECTOR (PX,PY)
.SH C-BINDING SYNOPSIS
#include <ncarg/ncargC.h>
.sp
void c_vector (float px, float py)
.SH DESCRIPTION 
.IP PX 12
(an input expression of type REAL) defines the X user coordinate.
.IP PY 12
(an input expression of type REAL) defines the Y user coordinate.
.SH C-BINDING DESCRIPTION
The C-binding argument descriptions are the same as the FORTRAN
argument descriptions.
.SH USAGE
Polyline type, line width, and color can be set by calling the
GKS routines GSLN, GSLWSC, GSCR, and GSPLCI before calling VECTOR.
.sp
Note that, for the sake of efficiency, the routines FRSTPT and VECTOR
buffer the polylines resulting from pen moves.  One must be sure to flush
the SPPS polyline buffer before changing polyline attributes.
A "CALL PLOTIF (0.,0.,2)" will flush the buffer.
.SH EXAMPLES
Use the ncargex command to see the following relevant examples: 
tgflas.
.SH ACCESS
To use VECTOR or c_vector, load the NCAR Graphics libraries ncarg, ncarg_gks,
and ncarg_c, preferably in that order.  
.SH SEE ALSO
Online:
gpl, gsln, gslwsc, gscr, gsplci,
spps, spps_params, frstpt, line, curve, ncarg_cbind
.sp
Hardcopy:  
NCAR Graphics Fundamentals, UNIX Version;
User's Guide for NCAR GKS-0A Graphics
.SH COPYRIGHT
Copyright (C) 1987-2007
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
