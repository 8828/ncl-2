.TH PCSETC 3NCARG "March 1993" UNIX "NCAR GRAPHICS"
.na
.nh
.SH NAME
PCSETC -  
Gives a new value to a specified internal parameter
of type CHARACTER.
.SH SYNOPSIS
CALL PCSETC (PNAM,CVAL)
.SH C-BINDING SYNOPSIS
#include <ncarg/ncargC.h>
.sp
void c_pcsetc (char *pnam, char *cval)
.SH DESCRIPTION 
.IP PNAM 12
(an input expression of type CHARACTER) specifies the name of the
parameter to be set. The name must appear as the first two
characters of the string. If the internal parameter is one
of the two (\'BC\' and \'CC\') that are arrays, the index of
the desired array element may appear, enclosed in
parentheses, in columns 3 and following. Other characters
may be used to document the use of the parameter being
retrieved; for example, instead of just \'MA\', one can use
\'MA - MAPPING FLAG\' and, instead of \'CC(10)\', one can use
\'CC(10) - SPECIAL COLOR 10\'.
.IP CVAL 12 
(an input expression of type CHARACTER)
is the value to be assigned to the
internal parameter specified by PNAM. 
.SH C-BINDING DESCRIPTION
The C-binding argument descriptions are the same as the FORTRAN 
argument descriptions.
.SH USAGE
This routine allows you to set the current value of
Plotchar parameters.  For a complete list of parameters available
in this utility, see the plotchar_params man page.
.SH EXAMPLES
Use the ncargex command to see the following relevant
example: 
epltch.
.SH ACCESS
To use PCSETC, load the NCAR Graphics libraries ncarg, ncarg_gks,
and ncarg_loc, preferably in that order.  To use c_pcsetc, load 
the NCAR Graphics libraries ncargC, ncarg_gksC, ncarg, ncarg_gks,
and ncarg_loc, preferably in that order.
.SH MESSAGES
See the plotchar man page for a description of all Plotchar error
messages and/or informational messages.
.SH SEE ALSO
Online:
plotchar,
plotchar_params,
pcdlsc,
pcgetc,
pcgeti,
pcgetr,
pchiqu,
pcloqu,
pcmequ,
pcmpxy,
pcpnwi,
pcseti,
pcsetr,
ncarg_cbind.
.SH COPYRIGHT
Copyright 1987, 1988, 1989, 1991, 1993 University Corporation
for Atmospheric Research
.br
All Rights Reserved
