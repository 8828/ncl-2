C
C $Id: arsetr.f,v 1.11 2000-08-22 15:01:54 haley Exp $
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
      SUBROUTINE ARSETR (IPN,RVL)
C
      CHARACTER*(*) IPN
C
C This subroutine is called to set the real value of a specified
C parameter.
C
C IPN is the name of the parameter whose value is to be retrieved.
C
C RVL is a real variable containing the desired new value.
C
C Declare the AREAS common block.
C
C
C ARCOMN contains variables which are used by all the AREAS routines.
C
      COMMON /ARCOMN/ IAD,IAU,ILC,RLC,ILM,RLM,ILP,RLP,IBS,RBS,DBS,IDB,
     +                IDC,IDI,IRC(16),RLA,RWA,RDI,RSI
      SAVE   /ARCOMN/
C
C Declare the BLOCK DATA routine external, which should force it to
C load from a binary library.
C
      EXTERNAL ARBLDA
C
C Define a character temporary to hold an error message.
C
      CHARACTER*38 CTM
C
C Check for an uncleared prior error.
C
      IF (ICFELL('ARSETR - UNCLEARED PRIOR ERROR',1).NE.0) RETURN
C
C Check for a parameter name that is too short.
C
      IF (.NOT.(LEN(IPN).LT.2)) GO TO 10001
        CTM(1:36)='ARSETR - PARAMETER NAME TOO SHORT - '
        CTM(37:36+LEN(IPN))=IPN
        CALL SETER (CTM(1:36+LEN(IPN)),2,1)
        RETURN
10001 CONTINUE
C
C Set the appropriate parameter value.
C
      IF (.NOT.(IPN(1:2).EQ.'AL'.OR.IPN(1:2).EQ.'al')) GO TO 10002
        RLA=RVL
      GO TO 10003
10002 CONTINUE
      IF (.NOT.(IPN(1:2).EQ.'AT'.OR.IPN(1:2).EQ.'at')) GO TO 10004
        IAD=MAX(0,INT(RVL))
        IAU=0
      GO TO 10003
10004 CONTINUE
      IF (.NOT.(IPN(1:2).EQ.'AW'.OR.IPN(1:2).EQ.'aw')) GO TO 10005
        RWA=RVL
      GO TO 10003
10005 CONTINUE
      IF (.NOT.(IPN(1:2).EQ.'DB'.OR.IPN(1:2).EQ.'db')) GO TO 10006
        IDB=INT(RVL)
      GO TO 10003
10006 CONTINUE
      IF (.NOT.(IPN(1:2).EQ.'DC'.OR.IPN(1:2).EQ.'dc')) GO TO 10007
        IDC=MAX(0,INT(RVL))
      GO TO 10003
10007 CONTINUE
      IF (.NOT.(IPN(1:2).EQ.'ID'.OR.IPN(1:2).EQ.'id')) GO TO 10008
        RDI=RVL
      GO TO 10003
10008 CONTINUE
      IF (.NOT.(IPN(1:2).EQ.'IS'.OR.IPN(1:2).EQ.'is')) GO TO 10009
        RSI=RVL
      GO TO 10003
10009 CONTINUE
      IF (.NOT.(IPN(1:2).EQ.'LC'.OR.IPN(1:2).EQ.'lc')) GO TO 10010
        ILC=MAX(1000,INT(RVL))
        IAU=0
      GO TO 10003
10010 CONTINUE
      IF (.NOT.(IPN(1:2).EQ.'RC'.OR.IPN(1:2).EQ.'rc')) GO TO 10011
        CALL ARGPAI (IPN,3,IPI)
        IF (.NOT.(IPI.EQ.0)) GO TO 10012
          DO 101 I=1,16
          IRC(I)=MAX(-2,MIN(2,INT(RVL)))
  101     CONTINUE
        GO TO 10013
10012   CONTINUE
        IF (.NOT.(IPI.GE.1.AND.IPI.LE.16)) GO TO 10014
          IRC(IPI)=MAX(-2,MIN(2,INT(RVL)))
        GO TO 10013
10014   CONTINUE
          CALL SETER ('ARSETR - ''RC'' INDEX IS OUT OF RANGE',3,1)
          RETURN
10013   CONTINUE
      GO TO 10003
10011 CONTINUE
        CTM(1:36)='ARSETR - PARAMETER NAME NOT KNOWN - '
        CTM(37:38)=IPN(1:2)
        CALL SETER (CTM(1:38),4,1)
        RETURN
10003 CONTINUE
C
C Done.
C
      RETURN
C
      END
