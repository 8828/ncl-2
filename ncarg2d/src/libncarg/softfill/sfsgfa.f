C
C $Id: sfsgfa.f,v 1.7 2003-05-20 20:52:12 kennison Exp $
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
      SUBROUTINE SFSGFA (XRA,YRA,NRA,DST,NST,IND,NND,ICI)
C
C Declare the dimensions of argument arrays.
C
      DIMENSION XRA(NRA),YRA(NRA),DST(NST),IND(NND)
C
C Declare the labeled common block.
C
      COMMON /SFCOMN/ AID,DBL,ITY,LPA,RDS,IDC,LCH,LDP(8,8)
C
C Declare the block data routine external to force its loading.
C
      EXTERNAL SFBLDA
C
C Check for an uncleared prior error.
C
      IF (ICFELL('SFSGFA - UNCLEARED PRIOR ERROR',1).NE.0) RETURN
C
C Do fill of the type selected by the values of ICI and ITY.
C
      IF (.NOT.(ITY.EQ.0)) GO TO 10001
      IF (.NOT.(ICI.GE.0)) GO TO 10002
      CALL GQFACI (IER,ISC)
      IF (.NOT.(IER.NE.0)) GO TO 10003
      CALL SETER ('SFSGFA - ERROR EXIT FROM GQFACI',2,1)
      RETURN
10003 CONTINUE
      CALL GSFACI (ICI)
10002 CONTINUE
      DO 10004 I=1,NRA
      XRA(I)=CUFX(XRA(I))
      IF (ICFELL('SFSGFA',3).NE.0) RETURN
      YRA(I)=CUFY(YRA(I))
      IF (ICFELL('SFSGFA',4).NE.0) RETURN
10004 CONTINUE
      CALL GETSET (XLVP,XRVP,YBVP,YTVP,XLWD,XRWD,YBWD,YTWD,LNLG)
      IF (ICFELL('SFSGFA',5).NE.0) RETURN
      CALL SET    (XLVP,XRVP,YBVP,YTVP,XLVP,XRVP,YBVP,YTVP,   1)
      IF (ICFELL('SFSGFA',6).NE.0) RETURN
      CALL GFA    (NRA,XRA,YRA)
      CALL SET    (XLVP,XRVP,YBVP,YTVP,XLWD,XRWD,YBWD,YTWD,LNLG)
      IF (ICFELL('SFSGFA',7).NE.0) RETURN
      IF (ICI.GE.0) CALL GSFACI (ISC)
      GO TO 10005
10001 CONTINUE
      IF (.NOT.(ITY.GT.0)) GO TO 10006
      IF (.NOT.(ICI.GE.0)) GO TO 10007
      CALL GQPLCI (IER,ISC)
      IF (.NOT.(IER.NE.0)) GO TO 10008
      CALL SETER ('SFSGFA - ERROR EXIT FROM GQPLCI',8,1)
      RETURN
10008 CONTINUE
      CALL GSPLCI (ICI)
10007 CONTINUE
      CALL SFWRLD (XRA,YRA,NRA,DST,NST,IND,NND)
      IF (ICFELL('SFSGFA',9).NE.0) RETURN
      IF (.NOT.(ITY.GT.1)) GO TO 10009
      AID=AID+90.
      CALL SFNORM (XRA,YRA,NRA,DST,NST,IND,NND)
      IF (ICFELL('SFSGFA',10).NE.0) RETURN
      AID=AID-90.
10009 CONTINUE
      IF (ICI.GE.0) CALL GSPLCI (ISC)
      GO TO 10005
10006 CONTINUE
      IF (.NOT.(ICI.GT.0)) GO TO 10010
      AIS=AID
      DBS=DBL
      IT1=-ITY
      DO 10011 I=1,IT1
      IT2=(ICI+IT1-I)/IT1
      IF (.NOT.(IT2.GT.0)) GO TO 10012
      AID=AIS+REAL((I-1)*(180/IT1))
      DBL=MAX(DBS/8.,DBS*2.**(6-IT2))
      IF (.NOT.(I.EQ.1)) GO TO 10013
      CALL SFWRLD (XRA,YRA,NRA,DST,NST,IND,NND)
      IF (ICFELL('SFSGFA',11).NE.0) RETURN
      GO TO 10014
10013 CONTINUE
      CALL SFNORM (XRA,YRA,NRA,DST,NST,IND,NND)
      IF (ICFELL('SFSGFA',12).NE.0) RETURN
10014 CONTINUE
10012 CONTINUE
10011 CONTINUE
      AID=AIS
      DBL=DBS
10010 CONTINUE
10005 CONTINUE
C
C Done.
C
      RETURN
C
      END
