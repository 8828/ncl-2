C
C $Id: pj21sp.f,v 1.2 2000-07-12 16:24:01 haley Exp $
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
      SUBROUTINE PJ21SP (COORD,CRDIO,INDIC)
C
C -- R O B I N S O N
C
      IMPLICIT REAL (A-Z)
      INTEGER IP1,NN
      INTEGER INDIC
      DIMENSION GEOG(2),PROJ(2),COORD(2),CRDIO(2),
     . PR(20),XLR(20)
C **** PARAMETERS **** A,LON0,X0,Y0 ************************************
      COMMON /ERRMZ0/ IERR
        INTEGER IERR
      SAVE   /ERRMZ0/
      COMMON /PRINZ0/ IPEMSG,IPELUN,IPPARM,IPPLUN
        INTEGER IPEMSG,IPELUN,IPPARM,IPPLUN
      SAVE   /PRINZ0/
      COMMON /PC21SP/ A,LON0,X0,Y0,PR,XLR
      DATA DG1 /0.01745329252E0/
      DATA PI /3.14159265358979323846E0/
      DATA EPSLN /1.0E-10/
C
C -- F O R W A R D   . . .
C
      IF (INDIC .EQ. 0) THEN
C
         GEOG(1) = COORD(1)
         GEOG(2) = COORD(2)
         IERR = 0
         LON = ADJLSP (GEOG(1) - LON0)
         P2=ABS(GEOG(2)/5.0E0/DG1)
         IP1=INT(P2-EPSLN)
C
C        STIRLING'S INTERPOLATION FORMULA (USING 2ND DIFF.)
C            USED WITH LOOKUP TABLE TO COMPUTE RECTANGULAR COORDINATES
C            FROM LAT/LONG.
C
         P2=P2-IP1
         X=A*(XLR(IP1+2)+P2*(XLR(IP1+3)-XLR(IP1+1))/2.0E0
     .     +P2*P2*(XLR(IP1+3)-2.0E0*XLR(IP1+2)+XLR(IP1+1))/2.0E0)*LON
         Y=A*(PR(IP1+2)+P2*(PR(IP1+3)-PR(IP1+1))/2.0E0
     .     +P2*P2*(PR(IP1+3)-2.0E0*PR(IP1+2)+PR(IP1+1))/2.0E0)*PI/2.0E0
     .     *SIGN(1.0E0,GEOG(2))
         PROJ(1) = X0 + X
         PROJ(2) = Y0 + Y
         CRDIO(1) = PROJ(1)
         CRDIO(2) = PROJ(2)
         RETURN
      END IF
C
C -- I N V E R S E   . . .
C
      IF (INDIC .EQ. 1) THEN
C
         PROJ(1) = COORD(1)
         PROJ(2) = COORD(2)
         IERR = 0
         X = PROJ(1) - X0
         Y = PROJ(2) - Y0
         YY = 2.0E0 * Y / PI / A
         PHID = YY * 90.0E0
         P2 = ABS(PHID / 5.0E0)
         IP1 = INT(P2 - EPSLN)
         IF (IP1.EQ.0) IP1 = 1
         NN = 0
C
C     STIRLING'S INTERPOLATION FORMULA AS USED IN FORWARD TRANSFORMATION
C     IS REVERSED FOR FIRST ESTIMATION OF LAT. FROM RECTANGULAR
C     COORDINATES.  LAT. IS THEN ADJUSTED BY ITERATION UNTIL USE OF
C     FORWARD SERIES PROVIDES CORRECT VALUE OF Y WITHIN TOLERANCE.
C
  230    U = PR(IP1 + 3) - PR(IP1 + 1)
         V = PR(IP1 + 3) - 2.0E0 * PR(IP1 + 2) + PR(IP1 + 1)
         T = 2.0E0 * (ABS(YY) - PR(IP1 + 2))/ U
         C = V / U
         P2 = T * (1.0E0 - C * T * (1.0E0 - 2.0E0 * C * T))
         IF (P2.LT.0.0E0.AND.IP1.NE.1) GO TO 240
         PHID = SIGN((P2 + IP1) * 5.0E0, Y)
  235    P2 = ABS(PHID / 5.0E0)
         IP1 = INT(P2 - EPSLN)
         P2 = P2 - IP1
         Y1=A*(PR(IP1+2)+P2*(PR(IP1+3)-PR(IP1+1))/2.0E0
     .     +P2*P2*(PR(IP1+3)-2.0E0*PR(IP1+2)+PR(IP1+1))/2.0E0)*PI/2.0E0
     .     * SIGN(1.0E0,Y)
         PHID = PHID - 180.0E0* (Y1 - Y) / PI / A
         NN = NN + 1
         IF (NN.LE.20) GO TO 237
         IF (IPEMSG .EQ. 0) WRITE (IPELUN,245)
         IERR = 214
         RETURN
  237    IF (ABS(Y1 - Y).GT.0.00001E0) GO TO 235
         GO TO 250
  240    IP1 = IP1 - 1
         GO TO 230
  245    FORMAT (/' ERROR PJ21SP'/
     .            ' TOO MANY ITERATIONS FOR INVERSE ROBINSON')
  250    GEOG(2) = PHID * DG1
C
C        CALCULATE LONG. USING FINAL LAT. WITH TRANSPOSED FORWARD
C        STIRLING'S INTERPOLATION FORMULA.
C
         GEOG(1)=LON0+X/A/(XLR(IP1+2)+P2*(XLR(IP1+3)-XLR(IP1+1))/2.0E0
     .     +P2*P2*(XLR(IP1+3)-2.0E0*XLR(IP1+2)+XLR(IP1+1))/2.0E0)
         GEOG(1) = ADJLSP(GEOG(1))
         CRDIO(1) = GEOG(1)
         CRDIO(2) = GEOG(2)
         RETURN
      END IF
C
      END
