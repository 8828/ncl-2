.\"
.\"	$Id: wmlabc.m,v 1.9 2005-01-04 15:42:34 haley Exp $
.\"
.TH WMLABC 3NCARG "January 1995" UNIX "NCAR GRAPHICS"
.SH NAME
WMLABC - plots weather map city names and daily hi/lows.
.SH SYNOPSIS
CALL WMLABC (X, Y, CITY, TEMPS)
.SH C-BINDING SYNOPSIS
#include <ncarg/gks.h>
.sp
void c_wmlabc(float x, float y, char *city, char *temps)
.SH DESCRIPTION
.IP X 12
(Real, Input) - An X coordinate (specified in world coordinates) of a
point that specifies the center of the composite label that consists 
of the city name in the character variable CITY and the temperature 
ranges in the character variable TEMPS.  The city name is plotted above
the temperatures.
.IP Y 12
(Real, Input) - A Y coordinate (specified in world coordinates) of a
point that specifies the center of the composite label that consists 
of the city name in the character variable CITY and the temperature 
ranges in the character variable TEMPS.  The city name is plotted above
the temperatures.
.IP CITY 12
(Character, Input) - A character variable that contains the name of the
city being plotted.
.IP TEMPS 12
(Character, Input) - A character variable that contains the daily hi/low
label for the city named in the third argument.
.SH USAGE
Set the values for the appropriate internal parameters before calling
WMLABC to produce the desired label.  The internal parameters that control
the appearance of the labels are: CBC, CHT, CMG, RFC.  To mark the 
cities' position with a dot, see the documentation for WMLABS.
.SH ACCESS
To use WMLABC or c_wmlabc, load the NCAR Graphics libraries ncarg, ncarg_gks, 
and ncarg_c, preferably in that order.  
.SH SEE ALSO
Online: 
wmap, wmdflt, wmgetc, wmgeti, wmgetr, wmlabs, wmsetc, wmseti, wmsetr, wmap_params
.sp
Hardcopy: 
WMAP - A Package for Producing Daily Weather Maps and Plotting Station 
Model Data
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
