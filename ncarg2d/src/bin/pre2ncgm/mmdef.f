C
C	$Id: mmdef.f,v 1.3 2000-07-12 17:04:34 haley Exp $
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
      SUBROUTINE MMDEF
C
C  SET THE REQUIRED DEFAULTS FOR THE MIMINAL METACODE TRANSLATOR
C
C  NOTE THAT XMDEF MUST BE CALLED PRIOR TO THE INVOCATION OF THIS ROUTINE
C
      COMMON /TRMTYP/ FOURBT, MULTBT, TWOBT, PTSFLG, CURCOL, IPWRIT,
     1                IFRAME, ISETOP, IRANG, IPTS, ICAS, IINT, IORNT,
     2                ISIZE, IFONT, IPAT, ISSIZ, ICENT, ICOLR,
     3                MCENT, MSIZE, MOR, OLDX, OLDY, MCASE, MINTEN,
     4                MFONT, MDASH, MSPOT, MCOLOR, MLIMIT, PTSYM,
     5                SAVX, SAVY
      INTEGER FOURBT, MULTBT, TWOBT, PTSFLG, CURCOL, IPWRIT,
     1                IFRAME, ISETOP, IRANG, IPTS, ICAS, IINT, IORNT,
     2                ISIZE, IFONT, IPAT, ISSIZ, ICENT, ICOLR,
     3                MCENT, MSIZE, MOR, OLDX, OLDY, MCASE, MINTEN,
     4                MFONT, MDASH, MSPOT, MCOLOR(3), MLIMIT(4), PTSYM,
     5                SAVX, SAVY
      COMMON /PGKSCOM/ CLPDAT, CLPFLG, POLIDX, LINTYP, LINWTH, LINCOL,
     1                LINRGB, MARIDX, MARSIZ, MARCOL, MARRGB, TXTIDX,
     2                INTSTL, PATIDX, FILCOL, FILRGB, MARTYP, HORIZ ,
     3                VERT  , PATH  , CHIGHT, XU    , YU    , XB    ,
     4                YB    , TXTCOL, FINDEX, CEXPN , CSPACE, CURIDX,
     5                CURIMP, CURINT, COLMOD, FILIDX, TXTRGB, PROPN ,
     6                FIRSTX, FIRSTY, LASTX , LASTY , TRATIO, CBV   ,
     7                CUV   , CHP   , CVP   , CCSPAC, CHHORZ, CDV   ,
     8                PMRSIZ, CLPX  , CLPY  , CLPP  , DEFREP, DEFLEN,
     9                VDCECR, TRANVN, TXTPRE, HATIDX, FILRPT, ASFSRF,
     A                ASFSDF, MAPMOD, VERSOK, PCBFST, CPGLEN, CLPNUL,
     B                MTDLEN
      INTEGER         CMPMAX, ASFMAX
      PARAMETER      (CMPMAX=256    , ASFMAX=18)
      LOGICAL         CLPFLG, PROPN , CHHORZ, DEFREP, CLPNUL, MAPMOD,
     1                VERSOK, PCBFST
      REAL            LINWTH, MARSIZ, CEXPN , CSPACE, TRATIO, CBV(2),
     1                CUV(2), CDV(2), CCSPAC
      INTEGER         CLPDAT(4)     , POLIDX, LINTYP, LINCOL, LINRGB(3),
     1                MARIDX, MARCOL, MARRGB(3)     , TXTIDX, INTSTL,
     2                PATIDX, FILCOL, FILRGB(3)     , MARTYP, HORIZ ,
     3                VERT  , PATH  , XU    , YU    , XB    , YB    ,
     4                TXTCOL, FINDEX, CURIMP(CMPMAX), CURINT(CMPMAX*3),
     5                CHIGHT, COLMOD, FILIDX, TXTRGB(3)     , CURIDX,
     6                FIRSTX, FIRSTY, LASTX , LASTY , CHP   , CVP   ,
     7                PMRSIZ, CLPX  , CLPY  , CLPP  , DEFLEN, TRANVN,
     8                TXTPRE, HATIDX, FILRPT(2)     , VDCECR(4)     ,
     9                ASFSRF(ASFMAX), ASFSDF(ASFMAX), CPGLEN, MTDLEN
      COMMON /GKSCHR/ MTDESC
      CHARACTER*80    MTDESC
      COMMON /TRTYPE/ METMIN, METEXT, METPRT, METHED, MIOPCL, MXOPCL,
     1                MIOPID, MXOPID, ASCDEC, ASCHEX, ASCOCT, BINARY,
     2                GAHNOR, GALEFT, GACENT, GARITE, GAVNOR, GATOP ,
     3                GACAP , GAHALF, GABASE, GABOTT, GRIGHT, GLEFT ,
     4                GUP   ,
     5                GDOWN , CINDEX, CDIRCT, PENUP , PENDN , FLAGVL,
     6                SEPRVL, ASCTEK, TYPRGB, TYPBGR,
     7                TYPHLS, TYPMON, SOLID , DASHED, INVSBL, SOLPAT,
     8                ASFIND, ASFBND, ASFLNT, ASFLNW, ASFLNC, ASFMRT,
     9                ASFMRS, ASFMRC, ASFFIS, ASFFHI, ASFFPI, ASFFCO,
     A                ASFFPT, ASFFPW, ASFFPC, ASFTFI, ASFTPR, ASFTCX,
     B                ASFTCS, ASFTCO, ASCFLT, PHOLLO, PSOLID, PPATTR,
     C                PHATCH, PEMPTY, HHORIZ, HVERTI, HPOSLP, HNESLP,
     D                HHOAVE, HPOANE
      INTEGER         METMIN, METEXT, METPRT, METHED, MIOPCL, MXOPCL,
     1                MIOPID, MXOPID, ASCDEC, ASCHEX, ASCOCT, BINARY,
     2                GAHNOR, GALEFT, GACENT, GARITE, GAVNOR, GATOP ,
     3                GACAP , GAHALF, GABASE, GABOTT, GRIGHT, GLEFT ,
     4                GUP   ,
     5                GDOWN , CINDEX, CDIRCT, PENUP , PENDN , FLAGVL,
     6                SEPRVL, ASCTEK, TYPRGB, TYPBGR,
     7                TYPHLS, TYPMON, SOLID , DASHED, INVSBL, SOLPAT,
     8                ASFIND, ASFBND, ASFLNT, ASFLNW, ASFLNC, ASFMRT,
     9                ASFMRS, ASFMRC, ASFFIS, ASFFHI, ASFFPI, ASFFCO,
     A                ASFFPT, ASFFPW, ASFFPC, ASFTFI, ASFTPR, ASFTCX,
     B                ASFTCS, ASFTCO, ASCFLT, PHOLLO, PSOLID, PPATTR,
     C                PHATCH, PEMPTY, HHORIZ, HVERTI, HPOSLP, HNESLP,
     D                HHOAVE, HPOANE
C
C  DEFAULT COLOR ATTRIBUTES TO FOREGROUND COLOR.
C
      CURCOL = 1
      CALL GSTXCI(CURCOL)
      CALL GSPLCI(CURCOL)
      CALL GSPMCI(CURCOL)
C
C  SET POINTS MODE OFF
C
      PTSFLG = 0
C
C  SET CHARACTER ORIENTATION TO 0 AND CENTERING TO LEFT
C
      MOR = 0
      MCENT = 0
C
C  SET THE DEFAULT DASH PATTERN TO SOLID
C
      MDASH = SOLID
      CALL PATSET(MDASH)
C
      RETURN
      END
