C
C $Id: mapchm.f,v 1.7 2000-07-12 16:23:38 haley Exp $
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
      SUBROUTINE MAPCHM (IPRT,IDPT,IAM,XCS,YCS,MCS,IAI,IAG,MAI,LPR)
C
      DIMENSION IAM(*),XCS(*),YCS(*),IAI(*),IAG(*)
C
C MAPCHM is called by various EZMAP routines to reset the color index,
C and dash pattern before and after drawing parts of a map by means
C of calls to ARDRLN (masked against an area map).
C
C The argument IPRT, if positive, says which part of the map is about
C to be drawn, as follows:
C
C     IPRT    Part of map.
C     ----    ------------
C       1     Perimeter.
C       2     Grid.
C       3     Labelling.
C       4     Limb lines.
C       5     Outline point group, continental.
C       6     Outline point group, U.S.
C       7     Outline point group, country.
C
C A call with IPRT equal to the negative of one of these values asks
C that the color index saved by the last call, with IPRT positive, be
C restored.
C
C When IPRT is positive, IDPT is the dash pattern to be used.  If IPRT
C is negative, IDPT is ignored.
C
C Declare required common blocks.  See MAPBD for descriptions of these
C common blocks and the variables in them.
C
      COMMON /MAPCM4/ INTF,JPRJ,PHIA,PHIO,ROTA,ILTS,PLA1,PLA2,PLA3,PLA4,
     +                PLB1,PLB2,PLB3,PLB4,PLTR,GRID,IDSH,IDOT,LBLF,PRMF,
     +                ELPF,XLOW,XROW,YBOW,YTOW,IDTL,GRDR,SRCH,ILCW,GRLA,
     +                GRLO,GRPO
      LOGICAL         INTF,LBLF,PRMF,ELPF
      SAVE   /MAPCM4/
C
      COMMON /MAPCMQ/ ICIN(8)
      SAVE   /MAPCMQ/
C
C Declare one of the dash-package common blocks, too.
C
      COMMON /SMFLAG/ ISMO
      SAVE   /SMFLAG/
C
C Certain variables need to be saved between calls.
C
      SAVE IPLS,IPMS,ISMS,ITXS
C
C Flush all buffers before changing anything.
C
      CALL MAPIQM (IAM,XCS,YCS,MCS,IAI,IAG,MAI,LPR)
      IF (ICFELL('MAPCHM',1).NE.0) RETURN
C
C Set/reset color index, dotting, and dash pattern.  The user has the
C last word.
C
      IF (.NOT.(IPRT.GT.0)) GO TO 10000
      ISMS=ISMO
      ISMO=1
      CALL DASHDB (IDPT)
      IF (ICFELL('MAPCHM',2).NE.0) RETURN
      IF (.NOT.(ICIN(IPRT).GE.0)) GO TO 10001
      CALL GQPLCI (IGER,IPLS)
      IF (.NOT.(IGER.NE.0)) GO TO 10002
      CALL SETER ('MAPCHM - ERROR EXIT FROM GQPLCI',3,1)
      RETURN
10002 CONTINUE
      CALL GQPMCI (IGER,IPMS)
      IF (.NOT.(IGER.NE.0)) GO TO 10003
      CALL SETER ('MAPCHM - ERROR EXIT FROM GQPMCI',4,1)
      RETURN
10003 CONTINUE
      CALL GQTXCI (IGER,ITXS)
      IF (.NOT.(IGER.NE.0)) GO TO 10004
      CALL SETER ('MAPCHM - ERROR EXIT FROM GQTXCI',5,1)
      RETURN
10004 CONTINUE
      CALL GSPLCI (ICIN(IPRT))
      CALL GSPMCI (ICIN(IPRT))
      CALL GSTXCI (ICIN(IPRT))
10001 CONTINUE
      CALL HLUMAPUSR (IPRT)
      IF (ICFELL('MAPCHM',6).NE.0) RETURN
      GO TO 10005
10000 CONTINUE
      CALL HLUMAPUSR (IPRT)
      IF (ICFELL('MAPCHM',7).NE.0) RETURN
      IF (.NOT.(ICIN(-IPRT).GE.0)) GO TO 10006
      CALL GSPLCI (IPLS)
      CALL GSPMCI (IPMS)
      CALL GSTXCI (ITXS)
10006 CONTINUE
      CALL DASHDB (IOR(ISHIFT(32767,1),1))
      IF (ICFELL('MAPCHM',8).NE.0) RETURN
      ISMO=ISMS
10005 CONTINUE
C
C Done.
C
      RETURN
C
      END
