C
C	$Id: tior.f,v 1.2 2000-07-12 17:04:28 haley Exp $
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
      SUBROUTINE TIOR(MASK,MSKDIM,IWDLNG,INTLNG,IOUT,IER)
      DIMENSION MASK(MSKDIM)
      IER=-1
      WRITE (IOUT,108)
      IWDP1=IWDLNG+1
      INTP1=INTLNG+1
      IF (MSKDIM.LT.IWDP1) GOTO 102
      IF (IWDLNG.LT.4 .OR. IWDP1.GT.99) GOTO 102
      IF (INTLNG.LT.4 .OR. INTLNG.GT.IWDLNG) GOTO 102
      IER=1
      IF (INTLNG.LT.IWDLNG) WRITE (IOUT,109) INTLNG
      K = IOR(0,0)
      IF (K.NE.0) GOTO 103
      K = IOR(3,10)
      IF (K.NE.11) GOTO 104
           DO 101 I=2,INTP1
           K = IOR(MASK(I),MASK(I-1))
           IF (K.NE.MASK(I)) GOTO 105
           K = IOR(MASK(I-1),MASK(I))
           IF (K.NE.MASK(I)) GOTO 106
  101      CONTINUE
      K=IOR(MASK(INTP1),MASK(INTP1))
      IF (K.NE.MASK(INTP1)) GOTO 107
      WRITE (IOUT,110)
      IER=0
      RETURN
  102 WRITE (IOUT,111) IWDLNG,INTLNG,MSKDIM
      RETURN
  103 WRITE (IOUT,112) K
      RETURN
  104 WRITE (IOUT,113) K
      RETURN
  105 WRITE (IOUT,114)
      CALL MSKOUT(' ',0,I-1,0,IOUT)
      WRITE (IOUT,115)
      IM2=I-2
      CALL MSKOUT(' ',1,IM2,0,IOUT)
      WRITE (IOUT,116) K
      IF (I.GT.2) WRITE (IOUT,117) IM2
      RETURN
  106 WRITE (IOUT,114)
      IM2=I-2
      CALL MSKOUT(' ',1,IM2,0,IOUT)
      WRITE (IOUT,115)
      CALL MSKOUT(' ',0,I-1,0,IOUT)
      WRITE (IOUT,116) K
      IF (I.GT.2) WRITE (IOUT,117) IM2
      WRITE (IOUT,118)
      CALL MSKOUT(' ',0,I-1,0,IOUT)
      WRITE (IOUT,119)
      CALL MSKOUT(' ',1,I-2,0,IOUT)
      WRITE (IOUT,120)
      CALL MSKOUT(' ',1,I-2,0,IOUT)
      WRITE (IOUT,119)
      CALL MSKOUT(' ',0,I-1,0,IOUT)
      RETURN
  107 WRITE (IOUT,114)
      CALL MSKOUT(' ',0,INTLNG,0,IOUT)
      WRITE (IOUT,115)
      CALL MSKOUT(' ',0,INTLNG,0,IOUT)
      WRITE (IOUT,116) K
      WRITE (IOUT,117) INTLNG
      RETURN
  108 FORMAT('1NOW TESTING IOR.')
  109 FORMAT ('0WARNING.  BECAUSE YOUR FORTRAN DOES NOT USE AN ',
     1     'ENTIRE WORD WHEN'/
     2     ' COMPARING OR ASSIGNING INTEGERS, IOR WILL BE TESTED ',
     3     'ONLY WITH THE'/
     4     ' LOWER ',I2,' BITS.')
  110 FORMAT ('0FINISHED TESTING IOR.  IOR APPARENTLY WORKS.')
  111 FORMAT ('0ERROR IN TEST PACKAGE DETECTED IN TIOR.  THE WORD ',
     1     'LENGTH MUST BE AT'/
     2     ' LEAST 4.  THE NUMBER OF BITS IN INTEGER COMPARISONS ',
     3     'AND ASSIGNMENTS'/
     4     ' MUST BE AT LEAST 4 AND .LE. WORD LENGTH.  AN I2 FORMAT ',
     5     'CONSTRAINS THE'/
     6     ' WORD LENGTH TO BE .LT. 99.  THE DIMENSION OF THE MASK ',
     7     'MUST BE .GT. WORD'/
     8     ' LENGTH.'/
     9     ' IWDLNG=',I16,'.  INTLNG=',I16,'.'/
     +     ' MSKDIM=',I16,'.')
  112 FORMAT ('0IOR DID NOT SUCCESSFULLY .OR. 0 WITH 0.  IT RETURNED ',
     1     'THE BINARY'/
     2     ' EQUIVALENT OF ',I16,'.')
  113 FORMAT ('0IOR DID NOT SUCCESSFULLY .OR. 0011 BINARY WITH 1010 ',
     1     'BINARY.  IT'/
     2     ' RETURNED THE BINARY EQUIVALENT OF ,I16,10H.  IOR DID'/
     3     ' SUCCESSFULLY .OR. 0 WITH 0.')
  114 FORMAT ('0IOR DID NOT SUCCESSFULLY .OR.')
  115 FORMAT (' WITH')
  116 FORMAT (' IT RETURNED THE BINARY EQUIVALENT OF ',I16,'.  IOR ',
     1     'DID'/
     2     ' SUCCESSFULLY .OR. 0 WITH 0, AND 0011 BINARY WITH 1010 ',
     3     'BINARY.')
  117 FORMAT ('0IOR ALSO PASSED THE TEST OF .OR.ING WORDS CONTAINING ',
     1     'ONES IN THE LOW'/
     2     ' ORDER K BITS WITH WORDS CONTAINING ONES IN THE LOW ORDER',
     3     ' K-1 BITS, FOR'/
     4     ' K=1 TO ',I2,'.  THIS TEST WAS APPLIED COMMUTATIVELY, SO ',
     5     'THAT THE FIRST'/
     6     ' ARGUMENT TO IOR CONTAINED K ONES AND THE SECOND ',
     7     'CONTAINED K-1.  THEN'/
     8     ' IOR WAS CALLED AGAIN WITH THE FIRST ARGUMENT CONTAINING',
     9     ' K-1 ONES, AND'/
     +     ' THE SECOND K ONES.')
  118 FORMAT ('0FURTHER, IOR SUCCESSFULLY PASSED THE COMMUTATIVE TEST',
     1     ' OF THE ONE IT'/
     2     ' FAILED.  THAT IS, IT WORKED WHEN THE FIRST ARGUMENT TO ',
     3     'IOR WAS')
  119 FORMAT (' AND THE SECOND WAS')
  120 FORMAT (' IT FAILED WHEN THE FIRST ARGUMENT WAS')
      END
