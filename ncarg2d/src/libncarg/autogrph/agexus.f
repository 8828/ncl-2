C
C $Id: agexus.f,v 1.5 2006-03-09 22:56:05 kennison Exp $
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
      SUBROUTINE AGEXUS (SVAL,ZMIN,ZMAX,ZLOW,ZHGH,
     +                               ZDRA,NVIZ,IIVZ,NEVZ,IIEZ,UMIN,UMAX)
C
      DIMENSION SVAL(2),ZDRA(1)
C
C The routine AGEXUS is used by AGSTUP to determine tentative values of
C the user-window edge coordinates.  Its arguments are as follows:
C
C -- SVAL is the array of special values.
C
C -- ZMIN and ZMAX are user-supplied minimum and maximum values of the
C    data x (or y) coordinates.
C
C -- ZLOW and ZHGH are, respectively, the smallest and largest data
C    values to be considered in choosing the minimum and maximum, if
C    those values, as given by the user, are null.
C
C -- ZDRA, NVIZ, IIVZ, NEVZ, and IIEZ specify the array of x (or y)
C    data coordinates (see AGMAXI or AGMINI for complete description).
C
C -- UMIN and UMAX are returned with tentative minimum and maximum
C    values for use at the appropriate user-window edges (left/right
C    or bottom/top).
C
C The following common block contains the new variable SMLL, which is
C essentially the smallest positive real number X such that "1.+X" is
C different from "1.".  This is used for some purposes for which the
C variable SMRL was previously used.  (10/21/89)
C
      COMMON /AGORIQ/ SMLL
      SAVE /AGORIQ/
C
C Assume initially that the user has provided actual values to be used.
C
      UMIN=ZMIN
      UMAX=ZMAX
C
C If either of the values is null, replace it by a data-based value.
C
      IF (UMIN.EQ.SVAL(1).OR.UMIN.EQ.SVAL(2))
     +                UMIN=AGMINI(SVAL(1),ZLOW,ZDRA,NVIZ,IIVZ,NEVZ,IIEZ)
      IF (UMAX.EQ.SVAL(1).OR.UMAX.EQ.SVAL(2))
     +                UMAX=AGMAXI(SVAL(1),ZHGH,ZDRA,NVIZ,IIVZ,NEVZ,IIEZ)
C
C Either or both values might still be null (if the user data was null).
C
      IF (UMIN.EQ.SVAL(1)) UMIN=UMAX
      IF (UMAX.EQ.SVAL(1)) UMAX=UMIN
C
C Check the relative values of UMIN and UMAX for problems.  One of the
C guiding principles here is that there should be no fewer than 500
C distinguishable real numbers between the minimum and the maximum.
C (10/20/90)
C
      UTMP=250.*SMLL*(ABS(UMIN)+ABS(UMAX))
      IF (ABS(UMIN-UMAX).LT.UTMP) GO TO 102
      IF (UMAX-UMIN) 101,102,103
  101 IF (ZMIN.NE.SVAL(1).AND.ZMIN.NE.SVAL(2)) UMAX=UMIN
      IF (ZMAX.NE.SVAL(1).AND.ZMAX.NE.SVAL(2)) UMIN=UMAX
C
  102 UAVG=.5*(UMIN+UMAX)
      UMIN=UAVG-.5*UTMP
      UMAX=UAVG+.5*UTMP
      IF (UMIN.NE.UMAX) GO TO 103
      UMIN=-1.
      UMAX=+1.
C
C If the user wanted these values back-stored, do it.
C
  103 IF (ZMIN.EQ.SVAL(2)) ZMIN=UMIN
      IF (ZMAX.EQ.SVAL(2)) ZMAX=UMAX
C
C Done.
C
      RETURN
C
      END
