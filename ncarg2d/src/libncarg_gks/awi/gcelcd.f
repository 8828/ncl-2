C
C	$Id: gcelcd.f,v 1.3 2000-07-12 16:39:39 haley Exp $
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
        SUBROUTINE GCELCD(WKID,IOS,STATUS)
C
C  Process the control elements.
C
C  At present the working control elements include:
C     VDC INTEGER PRECISION
C     CLIP RECTANGLE
C     CLIP INDICATOR
C  All other control elements will be ignored.
C
      include 'trinst.h'
      include 'trpars.h'
      include 'trstat.h'
      include 'trcode.h'
      include 'gkscom.h'
C
      INTEGER WKID, IOS, STATUS
      INTEGER TMP, XMIN, XMAX, YMIN, YMAX, CLPDAT(4)
C
      STATUS = 0
C
      IF (OPID .EQ. CELMVI) THEN
C
C  VDC integer precision.
C
        CALL GOPDEC(MOPLEN,MWHCPR,1,IOS,STATUS)
      ELSE IF (OPID .EQ. CELMCR) THEN
C
C  Set the clip rectangle for normalization transformation 1 that
C  we are currently using.  Transform the clipping rectangle if
C  IGSGCP is non-zero.
C
        CALL GOPDEC(CLPDAT,MOPLEN,4,IOS,STATUS)
        XMIN = CLPDAT(1)
        XMIN = MIN(XMIN,CLPDAT(3))
        XMAX = CLPDAT(3)
        XMAX = MAX(XMAX,CLPDAT(1))
        YMIN = CLPDAT(2)
        YMIN = MIN(YMIN,CLPDAT(4))
        YMAX = CLPDAT(4)
        YMAX = MAX(YMAX,CLPDAT(2))
        IF (IGSGCP .EQ. 0) THEN
          CALL GSWN(1,REAL(XMIN)/32767.,REAL(XMAX)/32767.,
     +                   REAL(YMIN)/32767.,REAL(YMAX)/32767.)
          CALL GSVP(1,REAL(XMIN)/32767.,REAL(XMAX)/32767.,
     +                   REAL(YMIN)/32767.,REAL(YMAX)/32767.)
        ELSE
          XMN = REAL(XMIN)/32767.
          XMX = REAL(XMAX)/32767.
          YMN = REAL(YMIN)/32767.
          YMX = REAL(YMAX)/32767.
          XM1 = CURTM(1,1)*XMN + CURTM(1,2)*YMN + CURTM(1,3)
          YM1 = CURTM(2,1)*XMN + CURTM(2,2)*YMN + CURTM(2,3)
          XM2 = CURTM(1,1)*XMX + CURTM(1,2)*YMX + CURTM(1,3)
          YM2 = CURTM(2,1)*XMX + CURTM(2,2)*YMX + CURTM(2,3)
          XM1 = MAX(0.,XM1)
          XM2 = MIN(1.,XM2)
          YM1 = MAX(0.,YM1)
          YM2 = MIN(1.,YM2)
          CALL GSWN(1,XM1,XM2,YM1,YM2)
          CALL GSVP(1,XM1,XM2,YM1,YM2)
        ENDIF
C
      ELSE IF (OPID.EQ.CELMCI) THEN
C
C  Set the clipping indicator.
C
        CALL GOPDEC(TMP,MENCPR,1,IOS,STATUS)
        CALL GSCLIP(TMP)
      END IF
C
C  Check the instruction status.
C
      IF (STATUS .NE. 0) RETURN
C
C  If the instruction is continued, then loop to get the 
C  full operand list.
C
  10  CONTINUE
      IF (CNTINU) THEN
        CALL GNPART(IOS,STATUS) 
        IF (STATUS .NE. 0) RETURN
        GO TO 10
      END IF 
C
      RETURN
      END
