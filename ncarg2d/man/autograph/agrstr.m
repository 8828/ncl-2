.TH AGRSTR 3NCARG "March 1993" UNIX "NCAR GRAPHICS"
.na
.nh
.SH NAME
AGRSTR - 
Restores a saved state of Autograph.
.SH SYNOPSIS
CALL AGRSTR (IFNO)
.SH C-BINDING SYNOPSIS
#include <ncarg/ncargC.h>
.sp
void c_agrstr (int ifno)
.SH DESCRIPTION
.IP IFNO 12
(an input expression of type INTEGER) is the number of
a unit from which a single unformatted record is to be
read. It is the user's responsibility to position this
unit. AGRSTR does not rewind it, either before or after
reading the record.
.SH C-BINDING DESCRIPTION
The C-binding argument description is the same as the FORTRAN 
argument description.
.SH USAGE
This routine restores internal parameter values from a file. 
For a complete list of parameters available
in this utility, see the autograph_params man page.
.SH ACCESS 
To use AGRSTR or c_agrstr, load the NCAR Graphics libraries ncarg, ncarg_gks, 
and ncarg_c, preferably in that order.  To get smoother curves, 
drawn using spline interpolation, also load libdashsmth.o.  Or,
you can use the ncargf77 command to compile your program and load 
the above libraries, then, to get smoother curves, use the 
-dashsmth option.
.SH MESSAGES
See the autograph man page for a description of all Autograph error
messages and/or informational messages.
.SH SEE ALSO
Online:
autograph,
autograph_params,
agback,
agbnch,
agchax,
agchcu,
agchil,
agchnl,
agcurv,
agdshn,
aggetc,
aggetf,
aggeti,
aggetp,
aggetr,
agpwrt,
agsave,
agsetc,
agsetf,
agseti,
agsetp,
agsetr,
agstup,
agutol,
anotat,
displa,
ezmxy,
ezmy,
ezxy,
ezy
.sp
Hardcopy:
NCAR Graphics Fundamentals, UNIX Version
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
