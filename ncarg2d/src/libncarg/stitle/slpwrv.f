C
C $Id: slpwrv.f,v 1.4 2000-08-22 15:06:34 haley Exp $
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
      SUBROUTINE SLPWRV (XPOS,YPOS,CHRS,SIZE)
C
C This routine is used by STITLE to write a message vertically.  The
C message is centered at the position (XPOS,YPOS) in the fractional
C coordinate system.  The characters are as specified by CHRS and they
C are of the size specified by SIZE (a character width, between 0 and
C 1, in the fractional coordinate system).
C
C Note: The current "user" coordinate system must be the fractional
C coordinate system for this routine to work properly.
C
        CHARACTER*(*) CHRS
C
C Find the length of the character string.
C
        NCHR=LEN(CHRS)
C
C Compute the appropriate size to tell WTSTR to use.
C
        ISIZ=INT(1023.*SIZE)
C
C Write the characters one at a time, using WTSTR.
C
        DO 101 I=1,NCHR
          CALL WTSTR (XPOS,YPOS+SIZE*(NCHR+1-2*I),CHRS(I:I),ISIZ,0,0)
          IF (ICFELL('SLPWRV',1).NE.0) RETURN
  101   CONTINUE
C
        RETURN
C
      END
