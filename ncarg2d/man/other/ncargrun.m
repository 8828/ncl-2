.\"
.\"	$Id: ncargrun.m,v 1.11 2000-07-11 23:03:59 haley Exp $
.\"
.TH NCARGRUN 1NCARG "JUNE 1988" NCAR "NCAR GRAPHICS"
.SH NAME
ncargrun \- to run a user program and redirect the metafile output
.SH SYNOPSIS
\fBncargrun\fP 
\fBprogram-name\fR
[\fB\-t\fR]
[\fB\-o metafile-name\fR]
.SH DESCRIPTION
.I ncargrun
is a script that invokes "program-name", which uses the NCAR GKS
library, and applies the specified options.
.LP
Prior to running your program, this script sets the environment variables
NCARG_GKS_OUTPUT and NCARG_GKS_PSOUTPUT to produce the desired effect.
.SH OPTIONS
.BI \-o " " "filename"

specifies the name of the output metafile or PostScript file.  If
filename is replaced by "| translator-name", the effect is to
pipe the metafile to the named translator.

.LP
.BI \-t " "
specifies that metafile output should be piped to the default translator
"ctrans".
.sp
.SH "SEE ALSO"
.BR ncarg_gks(1NCARG),
.BR ncarg_env(5NCARG),
.BR ncargintro(5NCARG)
.SH COPYRIGHT
Copyright (C) 1987-2000
.br
University Corporation for Atmospheric Research
.br

This documentation is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 2.1 of the
License, or (at your option) any later version.

This software is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this software; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
USA.
