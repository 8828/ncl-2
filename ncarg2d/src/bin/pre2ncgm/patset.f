C
C	$Id: patset.f,v 1.2 2000-07-12 17:04:35 haley Exp $
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
      SUBROUTINE PATSET(IPAT)
C
C  SET THE PATTERN FOR SOFTWARE DASHED LINE EMULATION
C
C  INPUT
C       IPAT-THE REQUESTED PATTERN
C               1 TO 4 PREDEFINED PATTERNS SET BY GKS
C               0 - A INVISIBLE LINE
C               ALL OTHERS THE 16 BIT VALUE DEFINES THE LOOK OF THE PATTERN
C
      COMMON /TRDASH/ PATFLG, DASPAT, PATBIT, DEFPAT, DASHX, DASHY,
     1                PATVDC
      INTEGER DPATSZ, PATSIZ, DFPTSZ
      PARAMETER (DPATSZ=16, PATSIZ=320, DFPTSZ=4)
      INTEGER PATFLG, DASPAT, PATBIT, DEFPAT(DFPTSZ), DASHX,
     1        DASHY, PATVDC
      COMMON /TRTYPE/ METMIN, METEXT, METPRT, METHED, MIOPCL, MXOPCL,
     1                MIOPID, MXOPID, ASCDEC, ASCHEX, ASCOCT, BINARY,
     2                GAHNOR, GALEFT, GACENT, GARITE, GAVNOR, GATOP ,
     3                GACAP , GAHALF, GABASE, GABOTT, GRIGHT, GLEFT ,
     4                GUP   ,
     5                GDOWN , CINDEX, CDIRCT, PENUP , PENDN , FLAGVL,
     6                SEPRVL, ASCTEK, TYPRGB, TYPBGR,
     7                TYPHLS, TYPMON, SOLID , DASHED, INVSBL, SOLPAT,
     8                ASFIND, ASFBND, ASFLNT, ASFLNW, ASFLNC, ASFMRT,
     9                ASFMRS, ASFMRC, ASFFIS, ASFFHI, ASFFPI, ASFFCO,
     A                ASFFPT, ASFFPW, ASFFPC, ASFTFI, ASFTPR, ASFTCX,
     B                ASFTCS, ASFTCO, ASCFLT, PHOLLO, PSOLID, PPATTR,
     C                PHATCH, PEMPTY, HHORIZ, HVERTI, HPOSLP, HNESLP,
     D                HHOAVE, HPOANE
      INTEGER         METMIN, METEXT, METPRT, METHED, MIOPCL, MXOPCL,
     1                MIOPID, MXOPID, ASCDEC, ASCHEX, ASCOCT, BINARY,
     2                GAHNOR, GALEFT, GACENT, GARITE, GAVNOR, GATOP ,
     3                GACAP , GAHALF, GABASE, GABOTT, GRIGHT, GLEFT ,
     4                GUP   ,
     5                GDOWN , CINDEX, CDIRCT, PENUP , PENDN , FLAGVL,
     6                SEPRVL, ASCTEK, TYPRGB, TYPBGR,
     7                TYPHLS, TYPMON, SOLID , DASHED, INVSBL, SOLPAT,
     8                ASFIND, ASFBND, ASFLNT, ASFLNW, ASFLNC, ASFMRT,
     9                ASFMRS, ASFMRC, ASFFIS, ASFFHI, ASFFPI, ASFFCO,
     A                ASFFPT, ASFFPW, ASFFPC, ASFTFI, ASFTPR, ASFTCX,
     B                ASFTCS, ASFTCO, ASCFLT, PHOLLO, PSOLID, PPATTR,
     C                PHATCH, PEMPTY, HHORIZ, HVERTI, HPOSLP, HNESLP,
     D                HHOAVE, HPOANE
      COMMON /TRMACH/ BTSWRD
      INTEGER BTSWRD
      COMMON /CAPDEV/ DGISTR, DGISIZ, DGESTR, DGESIZ, DTISTR,
     1                DTISIZ, DCDLLX, DCDLLY, DCDURX, DCDURY,
     3                DCOAVL, CORFMT, CORFIN, BATCH , DHCSIZ,
     4                DHCSTR, CORXOF, CORYOF, DASBIT, CORXSC,
     5                CORYSC, VDWLLX, VDWLLY, VDWURX, VDWURY
      INTEGER         DGIMAX, DGEMAX, DTIMAX, DCFTMX, DHCMAX
      PARAMETER   (DGIMAX=300, DGEMAX=150, DTIMAX=100)
      PARAMETER   (DCFTMX=30 , DHCMAX=50)
      INTEGER         DGISTR(DGIMAX), DGISIZ, DGESTR(DGEMAX),
     1                DGESIZ, DTISTR(DTIMAX), DTISIZ, DCDLLX,
     2                DCDLLY, DCDURX, DCDURY, CORFMT(DCFTMX,4),
     3                CORFIN(8)     , DHCSIZ, DHCSTR(DHCMAX),
     4                CORXOF, CORYOF, DASBIT, VDWLLX, VDWLLY,
     5                VDWURX, VDWURY
      REAL            CORXSC, CORYSC, CORRIN(8)
      LOGICAL         DCOAVL, BATCH
C  Size of the COMMON
      INTEGER         LENDEV
      PARAMETER   (LENDEV=DGIMAX+1+DGEMAX+1+DTIMAX+1+4+1+4*DCFTMX+
     1                  8+2+DHCMAX+9)
      EQUIVALENCE (CORFIN,CORRIN)
C
      INTEGER IPAT
C
C  IF PATTERN CLEAR THEN SET THE INVISIBLE FLAG
C
      IF (IPAT .EQ. INVSBL) THEN
        PATFLG = INVSBL
C
C  CHECK FOR SOLID PATTERN
C
      ELSE IF (IPAT.EQ.SOLID .OR. IPAT.EQ.SOLPAT) THEN
        PATFLG = SOLID
C
C  ELSE SET THE DASHED FLAG
C
      ELSE
        PATFLG = DASHED
        PATBIT = 1
        PATVDC = DASBIT
        DASHX = 0
        DASHY = 0
        IF (IPAT .GE. 2 .AND. IPAT .LT. 5) THEN
                CALL SBYTES(DASPAT, DEFPAT(IPAT), 0, DPATSZ, 0, 1)
        ELSE
                CALL SBYTES(DASPAT, IPAT, 0, DPATSZ, 0, 1)
        END IF
      END IF
C
      RETURN
      END
