C
C	$Id: gqlwk.f,v 1.4 2000-07-12 16:39:48 haley Exp $
C                                                                      
C                Copyright (C)  2000
C        University Corporation for Atmospheric Research
C                All Rights Reserved
C
C This file is free software; you can redistribute it and/or modify
C it under the terms of the GNU Lesser General Public License as
C published by the Free Software Foundation; either version 2.1 of the
C License, or (at your option) any later version.
C
C This software is distributed in the hope that it will be useful, but
C WITHOUT ANY WARRANTY; without even the implied warranty of
C MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
C Lesser General Public License for more details.
C
C You should have received a copy of the GNU Lesser General Public
C License along with this software; if not, write to the Free Software
C Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
C USA.
C
      SUBROUTINE GQLWK(WTYPE,ERRIND,MPLBTE,MPMBTE,MTXBTE,MFABTE,
     +                 MPAI,MCOLI)
C
C  INQUIRE MAXIMUM LENGTH OF WORKSTATION STATE TABLES
C
      include 'gkscom.h'
C
      INTEGER WTYPE,ERRIND,MPLBTE,MPMBTE,MTXBTE,MFABTE,MPAI,MCOLI
C
C  Check if GKS is in the proper state.
C
      CALL GZCKST(8,-1,ERRIND)
      IF (ERRIND .NE. 0) GOTO 100
C
C  Check that the workstation type is valid.
C
      CALL GZCKWK(22,-1,IDUM,WTYPE,ERRIND)
      IF (ERRIND .NE. 0) GO TO 100
C
C  Check if the workstation category is OUTPUT or OUTIN.
C
      CALL GQWKCA(WTYPE,ERRIND,ICAT)
      IF (ICAT.NE.GOUTPT .AND. ICAT.NE.GOUTIN) THEN
        ERRIND = 39
        GO TO 100
      ENDIF
C
      IF (WTYPE.EQ.GXWC .OR. WTYPE.EQ.GXWE .OR. WTYPE.EQ.GDMP) THEN
        MPLBTE = 0
        MPMBTE = 0
        MTXBTE = 0
        MFABTE = 0
        MPAI   = 0
        MCOLI  = 256
        RETURN
      ENDIF
C
      IF (WTYPE.GE.GPSMIN .AND. WTYPE.LE.GPSMAX) THEN
        DCUNIT = 1
        MPLBTE = 0
        MPMBTE = 0
        MTXBTE = 0
        MFABTE = 0
        MPAI   = 0
        MCOLI  = 256
        RETURN
      ENDIF
C
  100 CONTINUE
      MPLBTE = -1
      MPMBTE = -1
      MTXBTE = -1
      MFABTE = -1
      MPAI   = -1
      MCOLI  = -1
C
      RETURN
      END
