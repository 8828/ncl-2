C
C	$Id: g01pm.f,v 1.7 2003-02-13 23:58:22 fred Exp $
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
      SUBROUTINE G01PM
C
C  Process POLYMARKER element.
C
      include 'gksin.h'
      include 'g01io.h'
      include 'g01ins.h'
      include 'g01wsl.h'
      include 'g01rqa.h'
      include 'g01ast.h'
      include 'g01adc.h'
      include 'g01opc.h'
      include 'gksenu.h'
C
      INTEGER  KALL, IPRIM, NBYTES, NUMO, K, INDX1, INDX2
      DIMENSION NUMO(2)
      SAVE KALL
      DATA KALL/0/ , IPRIM/2/
C
      KALL = KALL+1
      IF  (KALL .EQ. 1) THEN
C
C  Set the WSL entry "DISPLAY SURFACE EMPTY" to "NOT EMPTY".
C
        MDEMPT = GNEMPT
C
C  Process pending attributes.
C
        IF (AGPEND(IPRIM)) THEN
C
C  Some changes are pending.
C
          IF (VALCHG(IVPLIX))  THEN
C
C  Send bundle index.
C
            NBYTES = 1+(MIXFW-1)/8
            CALL GPUTNI (CLMBIX, IDMBIX, NBYTES,  RERR)
            CALL GPUTPR (MRPMIX, MIXFW,  1, RERR)
            IF (RERR .NE. 0) RETURN
C
C  Set sent value to requested.  Clear change flag.
C
            MSPMIX         = MRPMIX
            VALCHG(IVPMIX) = .FALSE.
          END IF
          IF (VALCHG(IVMTYP)) THEN
C
C  MARKER TYPE
C
            NBYTES = 1+(MIXFW-1)/8
            CALL GPUTNI (CLMTYP, IDMTYP,  NBYTES,  RERR)
            CALL GPUTPR (MRMTYP, MIXFW,  1, RERR)
            IF (RERR .NE. 0) RETURN
C
C  Set sent value to requested.  Clear change flag.
C
            MSMTYP         = MRMTYP
            VALCHG(IVMTYP) = .FALSE.
          END IF
          IF (VALCHG(IVMSZS))  THEN
C
C  MARKER SIZE
C
            NBYTES = 1+(2*MCFPP-1)/8
            CALL GPUTNI (CLMSIZ, IDMSIZ, NBYTES,  RERR)
            CALL GFLCNV (ARMSZS,NUMO)
            CALL GPUTPR (NUMO, MCFPP,  2, RERR)
            IF (RERR .NE. 0) RETURN
C
C  Set sent value to requested.  Clear change flag.
C
            ASMSZS         = ARMSZS
            VALCHG(IVMSZS) = .FALSE.
          END IF
          IF (VALCHG(IVPMCI))  THEN
C
C  MARKER COLOR
C
            NBYTES = 1+(MCIXFW-1)/8
            CALL GPUTNI (CLMCLR, IDMCLR, NBYTES,  RERR)
            CALL GPUTPR (MRPMCI, MCIXFW,  1, RERR)
            IF (RERR .NE. 0) RETURN
C
C  Set sent value to requested.  Clear change flag.
C
            MSPMCI         = MRPMCI
            VALCHG(IVPMCI) = .FALSE.
          ENDIF
          IF (ANYASF)  THEN
C
C  Some ASF has changed.
C
            CALL G01SAS (IPRIM, RERR)
            IF (RERR.NE.0)  RETURN
          END IF
C
C  Clear aggregate change variable.
C
          AGPEND(IPRIM) = .FALSE.
        ENDIF
C
C  Treat first call, put out OPCODE, and points.
C
C  Put out opcode (CLASS and ID) and total LENGTH.
C
        NBYTES = 1+(2*RL1*MVDCFW-1)/8
        CALL GPUTNI (CLPMRK, IDPMRK, NBYTES, RERR)
C
C  Put out first points array.
C
C  Truncate points to limits of the NDC unit square, convert to VDC,
C  and store in MPXPY.
C
        DO 30 K=1,RL2
          INDX1 = 2*K-1
          INDX2 = INDX1+1
          MPXPY(INDX1) = MXOFF + IFIX(FLOAT(MXSCAL)*
     +                   (AMAX1(0.,AMIN1(1.0,RX(K)))))
          MPXPY(INDX2) = MYOFF + IFIX(FLOAT(MYSCAL)*
     +                   (AMAX1(0.,AMIN1(1.0,RY(K)))))
   30   CONTINUE
C
C  Send out points.
C
        CALL GPUTPR (MPXPY,  MVDCFW,     2*RL2, RERR)
        IF (RERR .NE. 0) RETURN
C
C  If there is to be no continuation, reset the parameter "KALL".
C
        IF (CONT .EQ. 0) THEN
          KALL = 0
          RETURN
        ENDIF
      ENDIF
C
C  Treat the continuation calls.
C
      IF (KALL .GT. 1) THEN
C
C  Truncate points to the limits of the NDC unit square, convert to 
C  VDC, and store in MPXPY.
C
        DO 40 K=1,RL2
          INDX1 = 2*K-1
          INDX2 = INDX1+1
          MPXPY(INDX1) = MXOFF + IFIX(FLOAT(MXSCAL)*
     +                   (AMAX1(0.,AMIN1(1.0,RX(K)))))
          MPXPY(INDX2) = MYOFF + IFIX(FLOAT(MYSCAL)*
     +                   (AMAX1(0.,AMIN1(1.0,RY(K)))))
   40   CONTINUE
        IF (CONT .EQ. 0) THEN
          CALL GPUTPR (MPXPY,  MVDCFW,     2*RL2, RERR)
          IF (RERR .NE. 0) RETURN
          KALL = 0
        ELSE
          CALL GPUTPR (MPXPY,  MVDCFW,     2*RL2, RERR)
          IF (RERR .NE. 0) RETURN
        ENDIF
      ENDIF
      RETURN
C
      END
