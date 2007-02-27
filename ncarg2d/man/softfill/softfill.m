.TH Softfill 3NCARG "March 1993" UNIX "NCAR GRAPHICS"
.na
.nh
.SH NAME
Softfill - Fills a polygonal subset of the plotter frame.
.SH SYNOPSIS
SFSGFA - Can be made to fill polygons by calling the GKS
routine GFA or by calling the lower-level Softfill routines 
SFWRLD and SFNORM.
.sp
SFWRLD - Fills polygons with parallel lines or with dots,
polymarkers, or selected characters arrayed in a regular
rectangular pattern.
.sp
SFNORM - Fills polygons with parallel lines or with dots,
polymarkers, or selected characters arrayed in a regular
rectangular pattern.
.sp
SFGETC - Retrieves the current character value of a specified
internal parameter. 
.sp
SFGETI - Retrieves the current integer value of a specified
internal parameter.
.sp
SFGETP - Retrieves the current value of the dot pattern.
.sp
SFGETR - Retrieves the current real value of a specified
internal parameter.
.sp
SFSETC - Sets the new character value of a specified internal
parameter.
.sp
SFSETI - Sets the new integer value of a specified internal
parameter.
.sp
SFSETP - Sets the new value of the dot pattern.
.sp
SFSETR - Sets the new real value of a specified internal
parameter.
.SH C-BINDING SYNOPSIS
c_sfsgfa
.br
c_sfwrld
.br
c_sfnorm
.br
c_sfgetc
.br
c_sfgeti
.br
c_sfgetp
.br
c_sfgetr
.br
c_sfsetc
.br
c_sfseti
.br
c_sfsetp
.br
c_sfsetr
.SH USER-MODIFIABLE INTERNAL ROUTINES
None
.SH MESSAGES
Various error conditions can occur in Softfill.  Each of these results in
a call to the error-handling routine SETER, with a final argument indicating
that the error is recoverable; by default, an error message is printed and
execution is terminated, but, if you turn on error recovery
(as described in the "man" page for "error_handling"), you
can get control back.
.sp
The error messages are as follows; all should be
more or less self-explanatory.
.sp
.in +5
SFGETC - PARAMETER NAME NOT KNOWN - X
.br
SFGETC - PARAMETER NAME TOO SHORT - X
.br
SFGETC - UNCLEARED PRIOR ERROR
.br
SFGETI - UNCLEARED PRIOR ERROR
.br
SFGETP - UNCLEARED PRIOR ERROR
.br
SFGETR - PARAMETER NAME NOT KNOWN - X
.br
SFGETR - PARAMETER NAME TOO SHORT - X
.br
SFGETR - UNCLEARED PRIOR ERROR
.br
SFNORM - ARRAY DST IS TOO SMALL
.br
SFNORM - ARRAY IND IS TOO SMALL
.br
SFNORM - COORDINATE ARRAYS TOO SMALL
.br
SFNORM - LOGIC ERROR - SEE SPECIALIST
.br
SFNORM - UNCLEARED PRIOR ERROR
.br
SFSETC - PARAMETER NAME NOT KNOWN - X
.br
SFSETC - PARAMETER NAME TOO SHORT - X
.br
SFSETC - UNCLEARED PRIOR ERROR
.br
SFSETI - UNCLEARED PRIOR ERROR
.br
SFSETP - UNCLEARED PRIOR ERROR
.br
SFSETR - PARAMETER NAME NOT KNOWN - X
.br
SFSETR - PARAMETER NAME TOO SHORT - X
.br
SFSETR - UNCLEARED PRIOR ERROR
.br
SFSGFA - ERROR EXIT FROM GQFACI
.br
SFSGFA - ERROR EXIT FROM GQPLCI
.br
SFSGFA - UNCLEARED PRIOR ERROR
.br
SFWRLD - UNCLEARED PRIOR ERROR
.in -5
.sp
The "LOGIC ERROR" in
SFNORM should never occur; if it does, it indicates a compiler
problem or some sort of tampering with the code of the package.
Error messages
that complain of an error exit from a GKS routine probably 
imply that GKS has somehow been put in the wrong state.
In the messages that result from using an incorrect internal-parameter
name in a call to one of the parameter-access routines, the "X" will be
replaced by the offending name.
.SH SEE ALSO
Online:
softfill_params, sfgetc, sfgeti, sfgetp, sfgetr, sfsetc, sfseti,
sfsetp, sfsetr, sfsgfa, sfwrld, ncarg_cbind
.sp
Hardcopy:
NCAR Graphics Fundamentals, UNIX Version
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
