.TH Softfill_params 3NCARG "March 1993" UNIX "NCAR GRAPHICS"
.na
.nh
.SH NAME
Softfill_params - This document briefly describes all 
Softfill internal parameters.
.SH DESCRIPTION 
Softfill has six internal parameters. For details, see 
the descriptions of the routines that are used to set and 
retrieve them. 
.sp
Parameter descriptions, in alphabetical order, of all Softfill
internal parameters follow. Each description begins with a line
giving the parameter name and the intrinsic FORTRAN type of the
parameter.
.IP "'AN' - Integer or Real"
Angle of fill lines, in degrees
counterclockwise from horizontal. The
default value is zero.
.IP "'CH' - Character or Integer"
Character selector (-n => GKS polymarker
n; 0 => dots; +n => CHAR(n)). The
default value is 0.
.IP "'DO' - Integer"
Dot fill selector (0 => line fill; 1 =>
dot fill).  The default value is 0,
which implies line fill.
.IP "'SP' - Real"
Spacing of fill lines, in the fractional
coordinate system. The default value is
0.00125.
.IP "'TY' - Integer"
Type of fill to be used by SFSGFA (-1 to
-4 => patterns using lines in 1 to 4
different directions and varying in
density as the argument ICI varies; 0 =>
color fill, by calling GFA, with ICI
specifying the color index; 1 =>
simulated color fill, using lines in one
direction, with ICI specifying the color
index; 2 => simulated color fill, using
lines in two directions, with ICI
specifying the color index). The default
value is 0.
.IP "'LDP' - Integer"
An 8x8 array of type INTEGER, containing
nothing but 0s and 1s, describing the
dot pattern to be used when 'DO' is set
non-zero to select dot fill, rather than
line fill. This internal parameter,
unlike the others, is accessed using the
routines SFGETP and SFSETP.
.SH SEE ALSO
Online:
sfgetc, sfgeti, sfgetp, sfgetr, sfsetc, sfseti, sfsetp, sfsetr
.sp
Hardcopy:
NCAR Graphics Fundamentals, UNIX Version
.SH COPYRIGHT
Copyright (C) 1987-2005
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
