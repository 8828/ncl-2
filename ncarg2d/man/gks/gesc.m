.\"
.\"	$Id: gesc.m,v 1.11 2003-05-25 17:16:50 haley Exp $
.\"
.TH GESC 3NCARG "March 1993" UNIX "NCAR GRAPHICS"
.SH NAME
GESC (Escape) - provides a standard way of implementing nonstandard 
GKS features.  NCAR GKS has only two user-accessible escape functions
defined: one for changing the name of a metafile and another for
pausing in an X window.
.SH SYNOPSIS
CALL GESC (FCTID, LIDR, IDR, MXODR, LODR, ODR)
.SH C-BINDING SYNOPSIS
#include <ncarg/gks.h>
.sp
void gescape(Gint func_id, const Gescape_in_data *in_data, Gstore *store_data,Gescape_out_data **out_data);
.SH DESCRIPTION
.IP FCTID 12
(Integer, Input) - 
A function identifier specifying the requested activity.  The legal
values for FCTID are "-1391" for changing a metafile name and "-1396"
for effecting a pause in an X window.
.IP LIDR 12
(Integer, Input) - 
Dimension of the IDR input data record array (for IDR, see below).
.IP "IDR" 12
(Character * 80 Array, Input) - Input data record.  For calls to GESC
with FCTID equal to -1391 the input data record should contain the
desired metafile name left justified and blank filled; for calls to
GESC with FCTID equal to -1396 the input data record should contain
the workstation identifier encoded as a five character number.
.IP MXODR 12
(Integer, Input) - Maximum length of the ODR output data record array (for
ODR, see below).
.IP LODR 12
(Integer, Output) - Dimension of the ODR output data record array.
.IP "ODR (MXODR)" 12
(Character * 80 Array, Output) -  Output data record.
.SH USAGE
LIDR, MXODR, and LODR must always be at least "1" in value.
.SH EXAMPLES
GESC can be used to dynamically change the name of
an output metafile and to do so one should use the calls
GOPKS and GOPWK instead of OPNGKS.  If you are using OPNGKS,
see the man page for SETUSV for changing the name of the metafile.
.sp
To change the name of the output metafile inside your program,
you should make a call similar to the following:
.nf

       CHARACTER*80 MFNAME

       CALL GOPKS (6,IDUM)
       MFNAME = 'new.cgm.name'
       CALL GESC(-1391,1,MFNAME,1,1,CDUM)

.fi
The call to GESC to change the metafile name must always occur just
before the call to GOPWK that opens a CGM workstation.  Setting the
environment variable NCARG_GKS_OUTPUT overrides any attempt to 
change the name of an output metafile via a GESC call.
.sp
See the "User's Guide for NCAR GKS-0A Graphics" for a more complete
example of changing metafile names from within a code.
.sp
Here is an example of a code that will create an X window, draw a
line, and then pause waiting for a mouse click or a key click.
.nf
   
       CHARACTER*80 IREC,ODUM
       CALL GOPKS(6,0)
       CALL GOPWK(3,0,8)
       CALL GACWK(3)
       CALL LINE(0.,0.,1.,1.)
       CALL SFLUSH
       IREC = '    3'
       CALL GESC(-1396,1,IREC,1,1,ODUM)
       CALL GDAWK(3)
       CALL GCLWK(3)
       CALL GCLKS
       STOP
       END

.fi
.sp
The functions FRAME and NGPICT are in general much easier to use and more
flexible than the direct ESCAPE call for pausing in an X window.  It is
suggested that those functions be used.
.SH ACCESS
To use GKS routines, load the NCAR GKS-0A library 
ncarg_gks.
.SH SEE ALSO
Online: 
SETUSV, FRAME, NGPICT, gescape
.sp
Hardcopy: 
User's Guide for NCAR GKS-0A Graphics;
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
