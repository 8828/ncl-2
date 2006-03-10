C
C $Id: isfill.f,v 1.4 2006-03-10 14:23:18 kennison Exp $
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
      SUBROUTINE ISFILL
C
C This routine fills the interior of the marked contour line in the
C screen model ISCR, OR's that screen model into the screen model ISCA,
C and then zeroes ISCR.
C
      COMMON /ISCOMN/ BIG,BIGD,DBPI,IDONE,IFILL,IFLIP,IONES,IHSS(1000),
     +                IREF,ISCA(16,256),ISCALE,ISCR(16,256),ISLBT,ISDR,
     +                LHSS,LX,NFPW,NINU,NINV,NINW,NX,NY,R0,RNX,RNY,
     +                SMALL,SVAL,TENSN,U,V,W,XMAX,XMIN,YMAX,YMIN,XVPL,
     +                XVPR,YVPB,YVPT
      SAVE   /ISCOMN/
C
      IF (IFILL.EQ.0) RETURN
C
      DO 105 IY=1,NY
        IV=0
        IZ=0
        DO 102 IW=1,LX
          IF (ISCR(IW,IY).EQ.0) THEN
            IF (IV.NE.0) ISCR(IW,IY)=IOR(ISCR(IW,IY),IONES)
          ELSE
            DO 101 IB=NFPW,1,-1
              IT=IAND(ISHIFT(ISCR(IW,IY),2*(1-IB)),3)
              IF (IT.EQ.2) THEN
                IV=3
                ISCR(IW,IY)=IOR(ISCR(IW,IY),ISHIFT(IV,2*(IB-1)))
              ELSE
                IF (IV.NE.0.AND.IT.NE.3)
     +                  ISCR(IW,IY)=IOR(ISCR(IW,IY),ISHIFT(IV,2*(IB-1)))
                IF (IT.EQ.1.AND.IV.EQ.0) IZ=1
                IF (IT.NE.0) IV=0
              END IF
  101       CONTINUE
          END IF
  102   CONTINUE
        IF (IZ.NE.0) THEN
          IV=0
          DO 104 IW=LX,1,-1
            IF (ISCR(IW,IY).EQ.0) THEN
              IF (IV.NE.0) ISCR(IW,IY)=IOR(ISCR(IW,IY),IONES)
            ELSE
              DO 103 IB=1,NFPW
                IT=IAND(ISHIFT(ISCR(IW,IY),2*(1-IB)),3)
                IF (IT.EQ.1) THEN
                  IV=3
                  ISCR(IW,IY)=IOR(ISCR(IW,IY),ISHIFT(IV,2*(IB-1)))
                ELSE
                  IF (IV.NE.0.AND.IT.NE.3)
     +                  ISCR(IW,IY)=IOR(ISCR(IW,IY),ISHIFT(IV,2*(IB-1)))
                  IF (IT.NE.0) IV=0
                END IF
  103         CONTINUE
            END IF
  104     CONTINUE
        END IF
  105 CONTINUE
C
      DO 107 IY=1,NY
        DO 106 IW=1,LX
          ISCA(IW,IY)=IOR(ISCA(IW,IY),ISCR(IW,IY))
          ISCR(IW,IY)=0
  106   CONTINUE
  107 CONTINUE
C
      RETURN
C
      END
