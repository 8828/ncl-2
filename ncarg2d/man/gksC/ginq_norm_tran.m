.\"
.\"	$Id: ginq_norm_tran.m,v 1.13 2005-01-04 15:42:11 haley Exp $
.\"
.TH GINQ_NORM_TRAN 3NCARG "March 1993" UNIX "NCAR GRAPHICS"
.SH NAME
ginq_norm_tran (Inquire normalization transformation) - retrieves the 
window and viewport associated with a given normalization transformation. 
Users of NCAR Graphics are encouraged to use the SPPS routines c_set
and c_getset to handle normalization transformations instead of the
GKS entries gset_win, gset_vp, gsel_norm_tran, and ginq_norm_tran.
.SH SYNOPSIS
#include <ncarg/gks.h>
.sp
void ginq_norm_tran(Gint num, Gint *err_ind, Gtran *norm_tran);
.SH DESCRIPTION
.IP num 12
(Input) - Gives the number of the normalization transformation.
.IP err_ind 12
(Output) - If the inquired value cannot be returned correctly,
a non-zero error indicator is returned in err_ind, otherwise a zero is returned.
Consult "User's Guide for NCAR GKS-0A Graphics" for a description of the
meaning of the error indicators.
.IP norm_tran.win 12
(Glimit, Output) - The world coordinate window limits.
.IP norm_tran.vp 12
(Glimit, Output) - The normalized device coordinate viewport limits.
.SH ACCESS
To use the GKS C-binding routines, load the ncarg_gks and
ncarg_c libraries.
.SH SEE ALSO
Online:
.BR gset_win(3NCARG),
.BR gset_vp(3NCARG),
.BR gsel_norm_tran(3NCARG),
.BR ginq_cur_norm_tran_num(3NCARG),
.BR ginq_max_norm_tran_num(3NCARG),
.BR getset(3NCARG),
.BR gset_clip_ind(3NCARG),
.BR ginq_clip(3NCARG),
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
