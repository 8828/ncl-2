.\"
.\"	$Id: ginq_colr_rep.m,v 1.13 2005-01-04 15:42:10 haley Exp $
.\"
.TH GINQ_COLR_REP 3NCARG "March 1993" UNIX "NCAR GRAPHICS"
.SH NAME
ginq_colr_rep (Inquire color representation) - retrieves the color value associated
with a color index on a particular workstation.
.SH SYNOPSIS
#include <ncarg/gks.h>
.sp
void ginq_colr_rep(Gint ws_id, Gint colr_ind, Ginq_type type, Gint *err_ind, Gcolr_rep  *colr_rep);
.SH DESCRIPTION
.IP ws_id 12
(Input) - Gives a workstation identifier as was set in
a previous call to gopen_ws.
.IP colr_ind 12
(Input) - A color index.
.IP type 12
(Input) - Specifies whether the returned color values are the
ones that were specified by a call to gset_colr_rep or whether they are the
color values that are actually being used on the specified workstation
(frequently a workstation will not be able to represent a requested 
color value precisely).
.RS
.IP GINQ_SET
Returns the color value set for index colr_ind either by default or by a call 
to gset_colr_rep.
.IP GINQ_REALIZED
Returns the color value actually used by the workstation to represent the 
requested color.
.RE
.IP err_ind 12
(Output) - If the inquired values cannot be returned correctly,
a non-zero error indicator is returned in err_ind, otherwise a zero is returned.
Consult "User's Guide for NCAR GKS-0A Graphics" for a description of the
meaning of the error indicators.
.IP colr_rep.rgb.red 12
(Gfloat, Output) - Returns an intensity value for red in the 
range 0. to 1. inclusive.
.IP colr_rep.rgb.green 12
(Gfloat, Output) - Returns an intensity value for green in the 
range 0. to 1. inclusive.
.IP colr_rep.rgb.blue 12
(Gfloat, Output) - Returns an intensity value for blue in the 
range 0. to 1. inclusive.
.SH USAGE
For details on setting and using color indices see the man page for gset_colr_rep.
.SH ACCESS
To use the GKS C-binding routines, load the ncarg_gks and
ncarg_c libraries.
.SH SEE ALSO
Online: 
.BR gset_colr_rep(3NCARG),
.BR gset_line_colr_ind(3NCARG),
.BR gset_marker_size(3NCARG),
.BR gset_text_colr_ind(3NCARG),
.BR gset_fill_colr_ind(3NCARG),
.BR gks(3NCARG),
.BR ncarg_gks_cbind(3NCARG)
.sp
Hardcopy: 
User's Guide for NCAR GKS-0A Graphics;
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
