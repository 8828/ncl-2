C
C	$Id: ctcell.f,v 1.2 2000-07-12 16:25:52 haley Exp $
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
      SUBROUTINE CTCELL (Z,MX,NX,NY,M,I0,J0)
C
C CTCELL COMPUTES LINES OF CONSTANT Z (CONTOUR LINES) IN ONE
C CELL OF THE ARRAY Z FOR THE SRFACE PACKAGE.
C Z,MX,NX,NY ARE THE SAME AS IN SRFACE.
C M          BY THE TIME CTCELL IS FIRST CALLED, M CONTAINS
C            THE TWO-SPACE PLOTTER LOCATION OF EACH Z POINT.
C            U(Z(I,J))=M(1,I,J).  V(Z(I,J))=M(2,I,J)
C I0,J0      THE CELL Z(I0,J0) TO Z(I0+1,J0+1) IS THE ONE TO
C            BE CONTOURED.
C
      DIMENSION       Z(MX,NY)   ,M(2,NX,NY)
      COMMON /SRFBLK/ LIMU(1024) ,LIML(1024) ,CL(41)     ,NCL        ,
     1                LL         ,FACT       ,IROT       ,NDRZ       ,
     2                NUPPER     ,NRSWT      ,BIGD       ,UMIN       ,
     3                UMAX       ,VMIN       ,VMAX       ,RZERO      ,
     4                IOFFP      ,NSPVAL     ,SPVAL      ,BIGEST
      DATA IDUB/0/
      R(HO,HU) = (HO-CV)/(HO-HU)
      I1 = I0
      I1P1 = I1+1
      J1 = J0
      J1P1 = J1+1
      H1 = Z(I1,J1)
      H2 = Z(I1,J1P1)
      H3 = Z(I1P1,J1P1)
      H4 = Z(I1P1,J1)
      IF (IOFFP .NE. 1) GO TO  10
      IF (H1.EQ.SPVAL .OR. H2.EQ.SPVAL .OR. H3.EQ.SPVAL .OR.
     1    H4.EQ.SPVAL) RETURN
   10 IF (AMIN1(H1,H2,H3,H4) .GT. CL(NCL)) RETURN
      DO 110 K=1,NCL
C
C FOR EACH CONTOUR LEVEL, DESIDE WHICH OF THE 16 BASIC SIT-
C UATIONS EXISTS, THEN INTERPOLATE IN TWO-SPACE TO FIND THE
C END POINTS OF THE CONTOUR LINE SEGMENT WITHIN THIS CELL.
C
         CV = CL(K)
         K1 = (IFIX(SIGN(1.,H1-CV))+1)/2
         K2 = (IFIX(SIGN(1.,H2-CV))+1)/2
         K3 = (IFIX(SIGN(1.,H3-CV))+1)/2
         K4 = (IFIX(SIGN(1.,H4-CV))+1)/2
         JUMP = 1+K1+K2*2+K3*4+K4*8
         GO TO (120, 30, 50, 60, 70, 20, 80, 90, 90, 80,
     1           40, 70, 60, 50, 30,110),JUMP
   20    IDUB = 1
   30    RA = R(H1,H2)
         MUA = FLOAT(M(1,I1,J1))+RA*FLOAT(M(1,I1,J1P1)-M(1,I1,J1))
         MVA = FLOAT(M(2,I1,J1))+RA*FLOAT(M(2,I1,J1P1)-M(2,I1,J1))
         RB = R(H1,H4)
         MUB = FLOAT(M(1,I1,J1))+RB*FLOAT(M(1,I1P1,J1)-M(1,I1,J1))
         MVB = FLOAT(M(2,I1,J1))+RB*FLOAT(M(2,I1P1,J1)-M(2,I1,J1))
         GO TO 100
   40    IDUB = -1
   50    RA = R(H2,H1)
         MUA = FLOAT(M(1,I1,J1P1))+RA*FLOAT(M(1,I1,J1)-M(1,I1,J1P1))
         MVA = FLOAT(M(2,I1,J1P1))+RA*FLOAT(M(2,I1,J1)-M(2,I1,J1P1))
         RB = R(H2,H3)
         MUB = FLOAT(M(1,I1,J1P1))+RB*FLOAT(M(1,I1P1,J1P1)-M(1,I1,J1P1))
         MVB = FLOAT(M(2,I1,J1P1))+RB*FLOAT(M(2,I1P1,J1P1)-M(2,I1,J1P1))
         GO TO 100
   60    RA = R(H2,H3)
         MUA = FLOAT(M(1,I1,J1P1))+RA*FLOAT(M(1,I1P1,J1P1)-M(1,I1,J1P1))
         MVA = FLOAT(M(2,I1,J1P1))+RA*FLOAT(M(2,I1P1,J1P1)-M(2,I1,J1P1))
         RB = R(H1,H4)
         MUB = FLOAT(M(1,I1,J1))+RB*FLOAT(M(1,I1P1,J1)-M(1,I1,J1))
         MVB = FLOAT(M(2,I1,J1))+RB*FLOAT(M(2,I1P1,J1)-M(2,I1,J1))
         GO TO 100
   70    RA = R(H3,H2)
         MUA = FLOAT(M(1,I1P1,J1P1))+
     1         RA*FLOAT(M(1,I1,J1P1)-M(1,I1P1,J1P1))
         MVA = FLOAT(M(2,I1P1,J1P1))+
     1         RA*FLOAT(M(2,I1,J1P1)-M(2,I1P1,J1P1))
         RB = R(H3,H4)
         MUB = FLOAT(M(1,I1P1,J1P1))+
     1         RB*FLOAT(M(1,I1P1,J1)-M(1,I1P1,J1P1))
         MVB = FLOAT(M(2,I1P1,J1P1))+
     1         RB*FLOAT(M(2,I1P1,J1)-M(2,I1P1,J1P1))
         IDUB = 0
         GO TO 100
   80    RA = R(H2,H1)
         MUA = FLOAT(M(1,I1,J1P1))+RA*FLOAT(M(1,I1,J1)-M(1,I1,J1P1))
         MVA = FLOAT(M(2,I1,J1P1))+RA*FLOAT(M(2,I1,J1)-M(2,I1,J1P1))
         RB = R(H3,H4)
         MUB = FLOAT(M(1,I1P1,J1P1))+
     1         RB*FLOAT(M(1,I1P1,J1)-M(1,I1P1,J1P1))
         MVB = FLOAT(M(2,I1P1,J1P1))+
     1         RB*FLOAT(M(2,I1P1,J1)-M(2,I1P1,J1P1))
         GO TO 100
   90    RA = R(H4,H1)
         MUA = FLOAT(M(1,I1P1,J1))+RA*FLOAT(M(1,I1,J1)-M(1,I1P1,J1))
         MVA = FLOAT(M(2,I1P1,J1))+RA*FLOAT(M(2,I1,J1)-M(2,I1P1,J1))
         RB = R(H4,H3)
         MUB = FLOAT(M(1,I1P1,J1))+RB*FLOAT(M(1,I1P1,J1P1)-M(1,I1P1,J1))
         MVB = FLOAT(M(2,I1P1,J1))+RB*FLOAT(M(2,I1P1,J1P1)-M(2,I1P1,J1))
         IDUB = 0
  100    CALL DRAWS (MUA,MVA,MUB,MVB,1,0)
         IF (IDUB)  90,110, 70
  110 CONTINUE
  120 RETURN
      END
