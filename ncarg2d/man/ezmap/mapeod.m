.TH MAPEOD 3NCARG "March 1993" UNIX "NCAR GRAPHICS"
.na
.nh
.SH NAME
MAPEOD - Called by the EZMAP routines MAPLOT and MAPBLA.  The default
version does nothing.  A user-written version may be supplied to examine
each outline-dataset segment and, perhaps, to delete selected ones.  Note
that this routine is not called by any of the EZMAPB routines; for the
same purpose, they call the routine MPCHLN.
.SH SYNOPSIS
CALL MAPEOD (NOUT,NSEG,IDLS,IDRS,NPTS,PNTS)
.SH DESCRIPTION 
.IP NOUT 12
(an input expression of type INTEGER) is the number of the outline
dataset from which the segment comes, as follows:
.RS
.IP NOUT 8
Dataset from which the segment came
.IP 1 8 
CO - Continental outlines only.
.IP 2 8 
US - U.S. state outlines only.
.IP 3 8 
PS - Continental, U.S. state, and international
outlines.
.IP 4 8 
PO - Continental and international outlines.
.RE
.IP NSEG 12
(an input expression of type INTEGER) is the number of the segment
within the outline dataset. The maps in the example named "mpex09" show
segment numbers for the outline dataset 'CO'; the program may be modified
to produce maps showing segment numbers for any outline dataset.
.IP "IDLS and IDRS" 12
(input expressions of type INTEGER) are identifiers for the
areas to the left and right, respectively, of the segment. (Left and
right are defined from the standpoint of a viewer standing at point 1 of
the segment and looking toward point 2.) 
.IP NPTS 12
(an input/output variable of type INTEGER), on entry, is the number
of points defining the outline segment. NPTS may be zeroed by MAPEOD to
suppress plotting of the segment on the map.
.IP PNTS 12
(an input/output array, dimensioned 2*NPTS, of type REAL) is an
array of coordinates. PNTS(1) and PNTS(2) are the latitude and longitude
of the first point, PNTS(3) and PNTS(4) the latitude and longitude of the
second point, ... PNTS(2*NPTS-1) and PNTS(2*NPTS) the latitude and
longitude of the last point. All values are in degrees. Longitudes are
all between -180 and +180; no segment crosses the meridian at -180 (+180)
degrees.
.SH USAGE
MAPEOD is called by MAPLOT to examine each segment in an outline dataset
just before it is plotted and by MAPBLA to examine each segment in an
outline dataset just before it is added to an area map. The default
version of MAPEOD does nothing. A user-supplied version may cause
selected segments to be deleted (to reduce the clutter in northern
Canada, for example).
.SH EXAMPLES
Use the ncargex command to see the following relevant
examples: 
mpex03,
mpex05,
mpex09.
.SH ACCESS
To use MAPEOD, load the NCAR Graphics libraries ncarg, ncarg_gks,
and ncarg_c, preferably in that order.  
.SH SEE ALSO
Online:
ezmap, 
ezmap_params, 
mapaci,
mapbla,
mapblm,
mapdrw,
mapfst,
mapgci,
mapgrd,
mapgrm,
mapgtc,  
mapgti,  
mapgtl,  
mapgtr,  
mapint,
mapiq,
mapiqa,
mapiqd,
mapiqm,
mapit,
mapita,
mapitd,
mapitm,
maplbl,
maplmb,
maplot,
mappos,   
maproj,  
maprs,
maprst,  
mapsav,  
mapset,  
mapstc,  
mapsti,  
mapstl,  
mapstr,  
maptra,
maptri,
maptrn,
mapusr, 
mapvec,
mpchln,
mpfnme,
mpgetc,
mpgeti,  
mpgetl,
mpgetr,  
mpglty,
mpiaty,
mpifnb,
mpilnb,
mpiola,
mpiosa,
mpipai,
mpipan,
mpipar,
mpisci,
mplnam,
mplndm,
mplndr,
mplnri,
mpname,
mprset,
mpsetc,  
mpseti,  
mpsetl,  
mpsetr,  
supmap,
supcon,
ncarg_cbind
.PP
Hardcopy:  
NCAR Graphics Contouring and Mapping Tutorial 
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
