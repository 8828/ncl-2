.\"
.\"	$Id: rasgetpal.m,v 1.14 2006-01-04 00:15:52 haley Exp $
.\"
.TH RASGETPAL 1NCARG "January 1993" NCARG "NCAR VIEW"
.SH NAME
rasgetpal \- extract the color palette of a rasterfile and write it
to standard output
.SH SYNOPSIS
\fBrasgetpal\fP 
[\fB\-Version\fR]
\fIsrcfile\fP
[\fIdstfile\fP]
.SH DESCRIPTION
.sp
Given \fIsrcfile\fP and no \fIdstfile\fP, \fBrasgetpal\fP will extract
the color palette from \fIsrcfile\fP and print it in textual form to
standard output. Given \fIsrcfile\fP and \fIdstfile\fP, \fBrasgetpal\fP
will extract the color palette from \fIsrcfile\fP and save it
in file \fIdstfile\fP. If the extension of \fIdstfile\fP is ".txt",
the color palette is saved in textual form. If it is ".pal", the
palette is saved in a binary HDF-compatible format.
.sp
Once you have a ".pal" color palette you can use it with NCSA's
XImage or any other application that uses this format of color
palette. A textual color palette can be edited using a standard
text editor and then fed back to \fIctrans\fP, \fIrasview\fP,
or \fIrascat\fP in order to get a modified color palette. It's
also useful when you simply want to know what's in your color
palette. See "man ras_palette" for more information on these
different formats.
.SH OPTIONS
.TP
.BI \-help
Print help information.
.TP
.BI \-Version
Print the version number.
.sp
.SH "EXAMPLE"
.LP
Let's suppose you have an X Window Dump rasterfile called \fIwindow.xwd\fP
and you'd like to get a textual copy of the color palette.
.sp
.in +3.0
.nf
% rasgetpal window.xwd window.txt
% vi window.txt /* edit the color table */
% rasview -pal window.txt window.xwd
.fi
.in -3.0
.sp
.LP
You could also use the command below to get the same palette file:
.sp
.in +3.0
.nf
% rasgetpal window.xwd >window.txt
.fi
.in -3.0
.sp
.LP
Now suppose you'd like to get an HDF-compatible binary palette
from "window.xwd":
.sp
.in +3.0
.nf
% rasgetpal window.xwd new.pal
.fi
.in -3.0
.sp
.SH "CAVEATS"
A color map can be extracted from indexed rasterfiles but not
from direct-color rasterfiles.
.sp
.SH "SEE ALSO"
.LP
\fBrasview\fP(1NCARG), \fBrascat\fP(1NCARG), \fBrasls\fP(1NCARG),
\fBrassplit\fP(1NCARG), \fBras_formats\fP(5NCARG),
\fBras_palette\fP(5NCARG)
.sp
Hardcopy:
NCAR Graphics Fundamentals, UNIX Version
.SH COPYRIGHT
Copyright (C) 1987-2006
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

