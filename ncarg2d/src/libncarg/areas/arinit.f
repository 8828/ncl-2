C
C $Id: arinit.f,v 1.13 2000-08-22 15:01:52 haley Exp $
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
      SUBROUTINE ARINIT (IER)
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
C Declare some needed double-precision variables.
C
      DOUBLE PRECISION DS1,DS2,DS3
C
C Decide what type of arithmetic to use.  These tests are somewhat
C heuristic and are based on experience with the package to date.
C
      IAU=IAD
C
      IF (.NOT.(IAU.EQ.0)) GO TO 10001
        CALL ARINI2 (ILC,RS1,RS2,RS3,DS1,DS2,DS3,RS4,RS5,RS6)
        IF (.NOT.(RS2.NE.RS1.AND.RS3.NE.RS2)) GO TO 10002
          IAU=1
        GO TO 10003
10002   CONTINUE
        IF (.NOT.(DS2.NE.DS1.AND.DS3.NE.DS2)) GO TO 10004
          IAU=2
        GO TO 10003
10004   CONTINUE
        IF (.NOT.(RS5.NE.RS4.AND.RS6.NE.RS5)) GO TO 10005
          IAU=3
        GO TO 10003
10005   CONTINUE
          IER=1
          RETURN
10003   CONTINUE
10001 CONTINUE
C
C If multiple precision integer arithmetic is to be used, decide what
C base to use.
C
      IF (.NOT.(IAU.GE.3)) GO TO 10006
        IBS=IAU
        IF (.NOT.(IBS.EQ.3)) GO TO 10007
          IBS=4
10008     CONTINUE
          IF (.NOT.(IBS*2.LT.I1MACH(9).AND.REAL(IBS*2)*REAL(IBS*2)+.25E0
     +.NE.REAL(IBS*2)*REAL(IBS*2))) GO TO 10009
            IBS=IBS*2
          GO TO 10008
10009     CONTINUE
10007   CONTINUE
10006 CONTINUE
C
C Set required secondary constants.
C
      ILM=ILC-1
      ILP=ILC+1
      RLC=REAL(ILC)
      RLM=REAL(ILM)
      RLP=REAL(ILP)
      RBS=REAL(IBS)
      DBS=DBLE(IBS)
C
C Done.
C
      IER=0
      RETURN
C
      END
