.TH IDSFFT 3NCARG "Marc 1993" UNIX "NCAR GRAPHICS"
.na
.nh
.SH NAME
IDSFFT - Performs smooth surface fitting when the projections
of the data points in the X-Y plane are irregularly distributed
in the plane.
.SH SYNOPSIS
 CALL IDSFFT (MD, NRAN, XRAN, YRAN, ZRAN, MREG, NREG, KREG, 
.br
+ XREG, YREG, ZREG, IWK, WK)
.SH C-BINDING SYNOPSIS
#include <ncarg/ncargC.h>
.sp
void c_idsfft (int md, int nran, float *xran, float *yran, \\
.br
float *zran, int mreg, int nreg, int kreg, float *xreg, \\
.br
float *yreg, float *zreg, int *iwk, float *wk)
.SH DESCRIPTION
.IP "MD" 12
(Integer, Input/output) - 
Mode of computation (must be 1, 
2, or 3).
.RS
.IP "1"
If this is the first call to this subroutine, or if the value 
of NRAN has been changed from the previous call, or if 
the contents of the XRAN or YRAN arrays have been 
changed from the previous call.
.IP "2"
If the values of NRAN and the XRAN, YRAN arrays are 
unchanged from the previous call, but new values for 
XREG, YREG are being used. If MD=2 and NRAN has been 
changed since the previous call to IDSFFT, an error return 
occurs.
.IP "3"
If the values of NRAN, MREG, NREG, XRAN, YRAN, XREG, 
YREG are unchanged from the previous call, that is, if the 
only change on input to IDSFFT is in the ZRAN array. If 
MD=3 and NRAN, MREG or NREG has been changed since the 
previous call to IDSFFT, an error return occurs.
.sp
Between the call with MD=2 or MD=3 and the preceding 
call, the IWK and WK work arrays should not be disturbed.
.RE
.IP "NRAN" 12
(Integer, Input) - 
Number of random data points (must be 4 
or greater).
.IP "XRAN(NRAN)" 12
(Real array, Input) - 
Array of dimension NRAN containing 
the X coordinates of the data points.
.IP "YRAN(NRAN)" 12
(Real array, Input) - 
Array of dimension NRAN containing 
the Y coordinates of the data points.
.IP "ZRAN(NRAN)" 12
(Real array, Input) - 
Array of dimension NRAN containing 
the Z coordinates of the data points.
.IP "MREG" 12
(Integer, Input) - 
Number of output grid points in the  
X-direction (must be 1 or greater).
.IP "NREG" 12
(Integer, Input) - 
Number of output grid points in the  
Y-direction (must be 1 or greater).
.IP "KREG" 12
(Integer, Input) - 
First dimension of ZREG 
as declared in the calling program. KREG�MREG, 
else an error return occurs.
.IP "XREG(MREG)" 12
(Real array, Input) - 
Array of dimension MREG containing 
the X coordinates of the output grid points.
.IP "YREG(NREG)" 12
(Real array, Input) - 
Array of dimension NREG containing 
the Y coordinates of the output grid points.
.IP "ZREG(KREG,NREG)" 12
(Real array, Output) - 
Real, two-dimensional array of 
dimension (KREG,NREG), storing the interpolated Z values 
at the output grid points.
.IP "IWK(*)" 12 
(Integer array, Workspace) - Integer work array 
of dimension at least 31 * NRAN + MREG * NREG.
.IP "WK(*)" 12 
(Real array, Workspace) - 
Real work array of 
dimension at least 6 * NRAN.
.SH ""
Inadequate work space IWK and WK may may cause incorrect results.
.sp
The data points must be distinct and their projections in the
X-Y plane must not be collinear; otherwise, an error return
occurs.
.SH C-BINDING DESCRIPTION
The C-binding argument descriptions are the same as the FORTRAN 
.SH EXAMPLES
To use IDSFFT routines, run "ncargex cbex01".  This will produce
several files in your directory, cbex01, cbex01.f, and cbex01.ncgm.
Edit cbex01.f and remove every line before the
line "C PACKAGE BIVAR".  What remains is the Bivar package.
.sp
Use the ncargex command to see the following relevant
examples: 
ccpcldm,
ccpfil,
ccplbam,
ccpllb,
ccpllc,
ccplll,
ccpllo,
ccpllp,
ccpllt,
ccpllw,
ccpnet,
ccppc,
ccppc1,
ccppc2,
ccppc3,
ccppc4,
ccprc,
ccpscam,
cidsfft,
fsfsgfa,
cbex01.
.SH ACCESS 
To use IDSFFT, load the NCAR Graphics libraries ncarg, ncarg_gks, 
ncarg_c, and ncarg_loc, preferably in that order.  To use c_idsfft, load 
the NCAR Graphics libraries ncargC, ncarg_gksC, ncarg, ncarg_gks, 
ncarg_c, and ncarg_loc, preferably in that order.
.SH SEE ALSO
Online:
bivar, idbvip, ncarg_cbind
.SH ACKNOWLEDGEMENTS
Bivar was written by Hiroshi Akima in august 1975 and rewritten
by him in late 1976.  In 1989 a new version of Bivar,
incorporating changes described in a Rocky Mountain Journal of
Mathematics was obtained from Dr. Akima, and included in NCAR
Graphics with his permission.
.SH COPYRIGHT
Copyright 1987, 1988, 1989, 1991, 1993 University Corporation
for Atmospheric Research
.br
All Rights Reserved
