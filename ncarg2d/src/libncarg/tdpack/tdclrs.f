C
C $Id: tdclrs.f,v 1.2 2000-07-12 16:26:31 haley Exp $
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
      SUBROUTINE TDCLRS (IWID,IBOW,SHDE,SHDR,IOFC,IOLC,ILMT)
C
C TDCLRS is called to do the GSCR calls defining the following colors
C on a specified workstation:
C
C   Color 0 - background color (black or white, depending on IBOW)
C   Color 1 - foreground color (white or black, depending on IBOW)
C   Color 2 - pure red
C   Color 3 - pure green
C   Color 4 - pure blue
C   Color 5 - pure cyan
C   Color 6 - pure magenta
C   Color 7 - pure yellow
C   Colors IOFC through IOLC - gray shades, from white to black
C   Colors IOFC+NSHD through IOLC+NSHD - shades of gray
C   Colors IOFC+2*NSHD through IOLC+2*NSHD - shades of red
C   Colors IOFC+3*NSHD through IOLC+3*NSHD - shades of green
C   Colors IOFC+4*NSHD through IOLC+4*NSHD - shades of blue
C   Colors IOFC+5*NSHD through IOLC+5*NSHD - shades of cyan
C   Colors IOFC+6*NSHD through IOLC+6*NSHD - shades of magenta
C   Colors IOFC+7*NSHD through IOLC+7*NSHD - shades of yellow
C
C (NSHD is equal to IOLC-IOFC+1, the number of elements in each block
C of color shades.)
C
C The arguments are as follows:
C
C   IWID is the workstation identifier.
C
C   If IBOW is 0, the foreground color is white and the background color
C   is black; if IBOW is non-zero, the opposite is true.
C
C   SHDE is a real value between 0 and 1, inclusive; values near 0 call
C   for more intense shades to be used, while values near 1 call for
C   more nearly pastel shades to be used.
C
C   SHDR is a real value between 0 and 1, inclusive; values near 0 say
C   that a narrower range of shades is to be used, while values near 1
C   say that a broader range of shades is to be used.
C
C   IOFC and IOLC are the first and last integers in a block of color
C   indices to be used for NSHD shades of gray ranging from pure white
C   to pure black (where NSHD=IOLC-IOFC+1).  The next NSHD indices (in
C   numerical order) will be used for the shades of gray selected by
C   SHDR and SHDE; the next NSHD indices after that for selected shades
C   of red, the next NSHD indices after that for selected shades of
C   green, and so on.
C
C   ILMT, if set to a value between 1 and 7, inclusive, says that only
C   that many blocks of NSHD indices will be defined.  For example, if
C   ILMT has the value 4, only the black-to-white scale and the shades
C   of gray, red, and green will be generated; shades of blue, cyan,
C   magenta, and yellow will not be.  (This allows one to have more
C   shades of each color at the expense of using fewer colors.)  Using
C   a value of ILMT less than 1 or greater than 7 will result in all
C   8*NSHD color shades being defined.
C
C The colors defined by calling TDCLRS may be used for any purpose, but
C they are particularly useful when calling TDPACK routines to render
C surfaces.
C
C Define the foreground and background colors.
C
        IF (IBOW.EQ.0) THEN
          CALL GSCR (IWID,0,0.,0.,0.)  !  black
          CALL GSCR (IWID,1,1.,1.,1.)  !  white
        ELSE
          CALL GSCR (IWID,0,1.,1.,1.)  !  white
          CALL GSCR (IWID,1,0.,0.,0.)  !  black
        END IF
C
C Define the pure colors.
C
        CALL GSCR (IWID,2,1.,0.,0.)    !  red
        CALL GSCR (IWID,3,0.,1.,0.)    !  green
        CALL GSCR (IWID,4,0.,0.,1.)    !  blue
        CALL GSCR (IWID,5,0.,1.,1.)    !  cyan
        CALL GSCR (IWID,6,1.,0.,1.)    !  magenta
        CALL GSCR (IWID,7,1.,1.,0.)    !  yellow
C
C Define the specified color shades.
C
        NSHD=IOLC-IOFC+1
C
        DO 101 IOCC=IOFC,IOLC
          P=1.-     REAL(IOCC-IOFC)/REAL(IOLC-IOFC)
          Q=1.-SHDR*REAL(IOCC-IOFC)/REAL(IOLC-IOFC)
          CALL GSCR (IWID,IOCC       ,     P,     P,     P) ! full range
          IF (ILMT.EQ.1) GO TO 101
          CALL GSCR (IWID,IOCC+  NSHD,     Q,     Q,     Q) ! grays
          IF (ILMT.EQ.2) GO TO 101
          CALL GSCR (IWID,IOCC+2*NSHD,     Q,SHDE*Q,SHDE*Q) ! reds
          IF (ILMT.EQ.3) GO TO 101
          CALL GSCR (IWID,IOCC+3*NSHD,SHDE*Q,     Q,SHDE*Q) ! greens
          IF (ILMT.EQ.4) GO TO 101
          CALL GSCR (IWID,IOCC+4*NSHD,SHDE*Q,SHDE*Q,     Q) ! blues
          IF (ILMT.EQ.5) GO TO 101
          CALL GSCR (IWID,IOCC+5*NSHD,SHDE*Q,     Q,     Q) ! cyans
          IF (ILMT.EQ.6) GO TO 101
          CALL GSCR (IWID,IOCC+6*NSHD,     Q,SHDE*Q,     Q) ! magentas
          IF (ILMT.EQ.7) GO TO 101
          CALL GSCR (IWID,IOCC+7*NSHD,     Q,     Q,SHDE*Q) ! yellows
  101   CONTINUE
C
C Done.
C
        RETURN
C
      END
