.TH CPGETR 3NCARG "March 1993" UNIX "NCAR GRAPHICS"
.na
.nh
.SH NAME
CPGETR - Retrieves the real value of an internal parameter of type REAL or
INTEGER.
.SH SYNOPSIS
CALL CPGETR (PNAM, RVAL)
.SH C-BINDING SYNOPSIS
#include <ncarg/ncargC.h>
.sp
void c_cpgetr (char *pnam, float *rval)
.SH DESCRIPTION 
.IP PNAM 12
(CHARACTER, input) is the name of a parameter whose 
real value is to be retrieved. Only the first three 
characters of PNAM are examined. It is recommended that the 
rest of the character string be used to improve the 
readability of the code. For example, instead of just 
\&'SFS\&', use \&'SFS - SCALE FACTOR SELECTOR\&'.
.IP RVAL 12
(REAL, output) is a variable in which the value of the 
parameter specified by PNAM is to be returned.
If the internal parameter is a value "r" of type REAL, the value returned
is "r".
If the internal parameter is a value "i" of type INTEGER, the value returned
is "REAL(i)".
.SH C-BINDING DESCRIPTION
The C-binding argument descriptions are the same as the FORTRAN 
argument descriptions.
.SH USAGE
This routine allows you to retrieve the current value of
Conpack parameters.  For a complete list of parameters available
in this utility, see the conpack_params man page.
.SH EXAMPLES
Use the ncargex command to see the following relevant
examples: 
ccpclc,
ccpklb,
ccpllt,
colcon,
cpex01,
cpex06,
cpex07,
cbex01.
.SH ACCESS
To use CPGETR or c_cpgetr, load the NCAR Graphics libraries ncarg, ncarg_gks,
and ncarg_c, preferably in that order.  
.SH MESSAGES
See the conpack man page for a description of all Conpack error
messages and/or informational messages.
.SH SEE ALSO
Online:
conpack,
conpack_params,
cpback, cpchcf, cpchcl, cpchhl, cpchil, cpchll, cpcica, cpclam, cpcldm,
cpcldr, cpcltr, cpcnrc, cpdrpl, cpezct, cpgetc, cpgeti, cplbam,
cplbdr, cpmpxy, cpmviw, cpmvrw, cppkcl, cppklb, cprect, cprset, cpscae,
cpsetc, cpseti, cpsetr, cpsps1, cpsps2, ncarg_cbind
.sp
Hardcopy:
NCAR Graphics Contouring and Mapping Tutorial
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

