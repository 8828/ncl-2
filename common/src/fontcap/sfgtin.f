C
C	$Id: sfgtin.f,v 1.3 2000-08-22 03:53:13 haley Exp $
C                                                                      
C			     Copyright (C)  1997
C	     University Corporation for Atmospheric Research
C			     All Rights Reserved
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
      SUBROUTINE SFGTIN(STRING, STRMAX, STRSIZ, IOS, STATUS)
C
C  Get a string of integers.
C
C  INPUT
C       STRMAX - Maximum number of integer values to transfer.
C
C  OUTPUT
C       STRING - The array of integers.
C       STRSIZ - The number of integers transfered.
C       IOS    - The I/O status, this value has meaning only if an 
C                error condition obtains (i.e. only if STATUS .GT. 0).
C       STATUS - Status indicator, see subroutine SFPRCF for details.
C
      include 'fnttab.h'
      include 'fnterr.h'
      include 'fntcom.h'
C
C  Define the maximum number of digits per integer allowed.
C
C  Note that the format at 80 must be changed to the same value
C  as INTMX.
C
      INTEGER INTMX
      PARAMETER (INTMX=6)
C
      INTEGER STRMAX, STRING(STRMAX), STRSIZ, IOS, STATUS
      INTEGER II, TPTR, NUMSZ
      CHARACTER*1 DECPT, MINUS, ZERO, NINE
      CHARACTER*1 BUF1(INTMX),BLANK
      CHARACTER*(INTMX) BUF2
C
      DATA DECPT, MINUS, ZERO, NINE /'.', '-', '0', '9'/
      DATA BLANK/' '/
C
C  Set string size to 0.
C
      STRSIZ = 0
C
C  Skip blanks.
C
 10   CONTINUE
      CALL SFSKBK(IOS, STATUS)
      IF (STATUS .NE. ALLOK) RETURN
C
C  Check for a numeric string.
C
      IF (LINE(IPTR:IPTR).NE.DECPT .AND. LINE(IPTR:IPTR).NE.MINUS .AND.       
     1   (LINE(IPTR:IPTR).LT.ZERO  .OR.  LINE(IPTR:IPTR).GT.NINE)) 
     2    RETURN
C
C  Process until a blank is encountered, or until the end of the line.
C
      NUMSZ = 0
      DO 50 II = IPTR, LSIZE
        IF (LINE(II:II) .NE.BLANK) THEN
          NUMSZ = NUMSZ + 1
C
C  Error - NUMSZ exceeds the maximum number of digits.
C
          IF (NUMSZ .GT. INTMX) THEN
            STATUS = MAXINT
            RETURN
          END IF
          BUF1(NUMSZ) = LINE(II:II)
          TPTR = II + 1
        ELSE
C
C  End of number.
C       
          GO TO 55
        END IF
 50   CONTINUE
C
C  Set pointer to current location.
C
 55   CONTINUE
      IPTR = TPTR
C
C  Zero out the conversion buffer.
C
      DO 60 II = 1,INTMX
        BUF2(II:II) = ZERO
 60   CONTINUE
C
C  Move string to conversion buffer.
C
       DO 70 II = 1,NUMSZ
        BUF2((INTMX-II+1):(INTMX-II+1)) = BUF1((NUMSZ-II)+1)
 70    CONTINUE
C
C  Convert to an integer.
C
      STRSIZ = STRSIZ + 1
      READ(BUF2,80,ERR=1000)STRING(STRSIZ)
 80   FORMAT(I6)
C
C  If the buffer is full, return.
C
      IF (STRSIZ .GE. STRMAX) RETURN
C
C  Continue to next element.
C
      GO TO 10
C
C  Error in formatting, set STATUS
C
 1000 CONTINUE
      STATUS = INTERR
      RETURN
      END
