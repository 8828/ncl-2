C
C $Id: sfgeti.f,v 1.5 2003-05-20 20:52:12 kennison Exp $
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
      SUBROUTINE SFGETI (CNP,IVP)
C
      CHARACTER*(*) CNP
C
C This subroutine is called to retrieve the integer value of a specified
C parameter.
C
C CNP is the name of the parameter whose value is to be retrieved.
C
C IVP is an integer variable in which the desired value is to be
C returned by SFGETI.
C
C
C Check for an uncleared prior error.
C
      IF (ICFELL('SFGETI - UNCLEARED PRIOR ERROR',1).NE.0) RETURN
C
C Use SFGETR to retrieve the real value, fix it, and return it to the
C user.
C
      CALL SFGETR (CNP,RVP)
      IF (ICFELL('SFGETI',2).NE.0) RETURN
      IVP=INT(RVP)
C
C Done.
C
      RETURN
C
      END
