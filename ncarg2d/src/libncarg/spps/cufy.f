C
C $Id: cufy.f,v 1.6 2006-03-10 00:25:33 kennison Exp $
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
      FUNCTION CUFY (RY)
C
C Given a Y coordinate RY in the user system, CUFY(RY) is a Y
C coordinate in the fractional system.
C
      COMMON /IUTLCM/ LL,MI,MX,MY,IU(96)
      SAVE /IUTLCM/
      DIMENSION WD(4),VP(4)
      CALL GQCNTN (IE,NT)
      IF (IE.NE.0) THEN
        CALL SETER ('CUFY - ERROR EXIT FROM GQCNTN',1,1)
        CUFY=0.
        RETURN
      END IF
      CALL GQNT (NT,IE,WD,VP)
      IF (IE.NE.0) THEN
        CALL SETER ('CUFY - ERROR EXIT FROM GQNT',2,1)
        CUFY=0.
        RETURN
      END IF
      I=3
      IF (MI.EQ.2.OR.MI.GE.4) I=4
      IF (LL.LE.1.OR.LL.EQ.3) THEN
        CUFY=(RY-WD(I))/(WD(7-I)-WD(I))*(VP(4)-VP(3))+VP(3)
      ELSE
        CUFY=(ALOG10(RY)-WD(I))/(WD(7-I)-WD(I))*(VP(4)-VP(3))+VP(3)
      ENDIF
      RETURN
      END
