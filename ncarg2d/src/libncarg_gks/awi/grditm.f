C
C	$Id: grditm.f,v 1.4 2000-08-22 15:08:16 haley Exp $
C                                                                      
C                Copyright (C)  2000
C        University Corporation for Atmospheric Research
C                All Rights Reserved
C
C This file is free software; you can redistribute it and/or modify
C it under the terms of the GNU General Public License as published
C by the Free Software Foundation; either version 2 of the License, or
C (at your option) any later version.
C
C This software is distributed in the hope that it will be useful, but
C WITHOUT ANY WARRANTY; without even the implied warranty of
C MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
C General Public License for more details.
C
C You should have received a copy of the GNU General Public License
C along with this software; if not, write to the Free Software
C Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
C USA.
C
      SUBROUTINE GRDITM (WKID,MLDR,LDR,DATREC)
C
C  READ ITEM FROM GKSM
C
      INTEGER ERDITM
      PARAMETER (ERDITM=103)
C
      include 'gkscom.h'
C
      INTEGER WKID,LDR
      CHARACTER*80 DATREC(MLDR)
C
C  Check if GKS is in the proper state.
C
      CALL GZCKST(7,ERDITM,IER)
      IF (IER .NE. 0) RETURN
C
C  Check if the workstation ID is valid.
C
      CALL GZCKWK(20,ERDITM,WKID,IDUM,IER)
      IF (IER .NE. 0) RETURN
C
C  Check if the workstation is currently open.
C
      CALL GZCKWK(25,ERDITM,WKID,IDUM,IER)
      IF (IER .NE. 0) RETURN
C
C  Set function code and put out WKID, TYPE, LDR, and the data
C  record in STR.
C
C  Set the flag CUFLAG to indicate that the interface call should go
C  only to the specifically designated workstation.
C
      CUFLAG = WKID
      FCODE = 103
      CONT  = 0
      CALL GZROI(0)
      IL1 = 2
      IL2 = 2
      ID(1) = WKID
      ID(2) = MLDR
      CALL GZTOWK
      CUFLAG = -1
      IF (RERR .NE. 0) THEN
        ERS = 1
        CALL GERHND(RERR,ERDITM,ERF)
        ERS = 0
        RETURN
      ELSE
C
C  Read returned data record.
C
      LDR = ID(3)
C
C  Check on returned length.
C
      IF (LDR .GT. MLDR) THEN
        ERS = 1
        CALL GERHND(2001,ERDITM,ERF)
        ERS = 0
        RETURN
      ENDIF
      DATREC(1) = STR(1:80)
      IF (LDR .GT. 1) THEN
        INDX = 1
  200   CONTINUE
        INDX = INDX+1
        CALL GZFMWK
        DATREC(INDX) = STR(1:80)
        IF (CONT.EQ.0 .OR. INDX.GE.LDR) GO TO 201
          GO TO 200
  201     CONTINUE
        ENDIF
      ENDIF
C
      RETURN
      END
