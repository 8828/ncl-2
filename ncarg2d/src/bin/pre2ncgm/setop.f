C
C	$Id: setop.f,v 1.4 2000-07-12 17:04:36 haley Exp $
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
      SUBROUTINE SETOP(IOS,STATUS)
C
C  SET THE REQUESTED OPTION VALUE
C
C  OUTPUT
C       IOS- THE I/O STATUS VALID IF STATUS SET TO AN I/O ERROR
C       STATUS-THE ERROR STATUS DEFINED BY COMMON TREROR
C
      COMMON/TREROR/ ALLOK, MFRCHK, MTOPER, METRDC, REDERR, TYPCHG
     1             ,INVTYP, MINVLD, TYPERR, FRMEND, ENCINT, IVDCDT
     2             ,GCOERR, GCRERR, GCCERR, FCOERR, FCRERR, FCCERR
     3             ,PLIDXG, PMIDXG, TXIDXG, PGIDXG, INVLMT, CELERR
     4             ,COIERR, COLNRM, UNKNOW, UNKOPC, ENDMTF, VNEROR
     5             ,BADRSZ, DEVOUT, NOVERS, BADFNT, PGMERR, FASERR
     6             ,HINERR, VDWERR, RDWERR, RIXLIM
      INTEGER        ALLOK, MFRCHK, MTOPER, METRDC, REDERR, TYPCHG
     1             ,INVTYP, MINVLD, TYPERR, FRMEND, ENCINT, IVDCDT
     2             ,GCOERR, GCRERR, GCCERR, FCOERR, FCRERR, FCCERR
     3             ,PLIDXG, PMIDXG, TXIDXG, PGIDXG, INVLMT, CELERR
     4             ,COIERR, COLNRM, UNKNOW, UNKOPC, ENDMTF, VNEROR
     5             ,BADRSZ, DEVOUT, NOVERS, BADFNT, PGMERR, FASERR
     6             ,HINERR, VDWERR, RDWERR, RIXLIM
      COMMON/TRINOT/ IPTR, MBUFOT, MAXBYT, DEVUNT, METUNT,
     1          METIPT, ERRUNT, FNTUNT
      INTEGER MAXCNT
      PARAMETER (MAXCNT=200)
      INTEGER IPTR, MBUFOT(MAXCNT), MAXBYT, DEVUNT, METUNT, ERRUNT,
     1        FNTUNT
      LOGICAL METIPT
      COMMON /TRINST/ OPCL, OPID, LEN, CONT
      INTEGER OPCL, OPID, LEN
      LOGICAL CONT
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
      COMMON /CAPLIN/ PLAVBL, LDSSTR, LDSSIZ, LDTSTR, LDTSIZ,
     1                LMSSTR, LMSSIZ, LMTSTR, LMTSIZ, LCSSTR,
     2                LCSSIZ, LCTSTR, LCTSIZ, LINFIN, LINFMT,
     3                LWSSTR, LWSSIZ, LWTSTR, LWTSIZ, LWTFIN,
     4                LWTFMT, LWTRNG, LWTSCF, LBSSTR, LBSSIZ,
     5                LBTSTR, LBTSIZ, LPSSTR, LPSSIZ, LPTSTR,
     6                LPTSIZ
      INTEGER         LDSMAX, LDTMAX, LMSMAX, LMTMAX, LCSMAX,
     1                LCTMAX, LVCFMX, LWSMAX, LWTMAX, LWTFMX,
     2                LBSMAX, LBTMAX, LPSMAX, LPTMAX
      PARAMETER   (LDSMAX=30, LDTMAX=15, LMSMAX=30, LMTMAX=15,
     1             LCSMAX=30, LCTMAX=15, LVCFMX=8,  LWSMAX=30,
     2             LWTMAX=15, LWTFMX=8 , LBSMAX=30, LBTMAX=15,
     3             LPSMAX=20, LPTMAX=20)
      INTEGER         LDSSTR(LDSMAX), LDSSIZ, LDTSTR(LDTMAX),
     1                LDTSIZ, LMSSTR(LMSMAX), LMSSIZ,
     2                LMTSTR(LMTMAX), LMTSIZ, LCSSTR(LCSMAX),
     3                LCSSIZ, LCTSTR(LCTMAX), LCTSIZ, LINFIN(8),
     4                LINFMT(LVCFMX,4)      , LWSSTR(LWSMAX),
     5                LWSSIZ, LWTSTR(LWTMAX), LWTSIZ, LWTFIN(8),
     6                LWTFMT(LWTFMX,4)      , LWTRNG(2)     ,
     7                LBSSTR(LBSMAX), LBSSIZ, LBTSTR(LBTMAX),
     8                LBTSIZ, LPSSTR(LPSMAX), LPSSIZ,
     9                LPTSTR(LPTMAX), LPTSIZ
      LOGICAL         PLAVBL
      REAL            LWTSCF,LINRIN(8),LWTRIN(8)
      INTEGER         LENLIN
      PARAMETER   (LENLIN=1+LDSMAX+1+LDTMAX+1+LMSMAX+1+
     1             LMTMAX+1+LCSMAX+1+LCTMAX+1+8+LVCFMX*4+
     2             LWSMAX+1+LWTMAX+1+8+LWTFMX*4+2+1+LBSMAX+
     3             1+LBTMAX+1+LPSMAX+1+LPTMAX+1)
      EQUIVALENCE (LINFIN,LINRIN), (LWTFIN,LWTRIN)
      COMMON /CINFO/COLVLS,INTS,NUMI
      DIMENSION COLVLS(3,256),INTS(256)
C
      INTEGER IOS, STATUS
      INTEGER BYTE8, W16, OCOUNT, OPCODE, NSKIP, K
C
      REAL RGBV(3,16)
      CHARACTER*1 CINT(10)
      CHARACTER*3 CINDX,CVAL(16)
      INTEGER CUSED(16)
      SAVE
      DATA CINT /'0','1','2','3','4','5','6','7','8','9'/
      DATA CVAL /'BLA','WHI','RED','GRE','BLU','CYA','MAG','YEL',
     -           'ORA','GRA','VIO','TAN','BRO','GOL','PIN','TUR'/
      DATA RGBV(1, 1),RGBV(2, 1),RGBV(3, 1) /0.00, 0.00, 0.00/
      DATA RGBV(1, 2),RGBV(2, 2),RGBV(3, 2) /1.00, 1.00, 1.00/
      DATA RGBV(1, 3),RGBV(2, 3),RGBV(3, 3) /1.00, 0.00, 0.00/
      DATA RGBV(1, 4),RGBV(2, 4),RGBV(3, 4) /0.00, 1.00, 0.00/
      DATA RGBV(1, 5),RGBV(2, 5),RGBV(3, 5) /0.00, 0.00, 1.00/
      DATA RGBV(1, 6),RGBV(2, 6),RGBV(3, 6) /0.00, 1.00, 1.00/
      DATA RGBV(1, 7),RGBV(2, 7),RGBV(3, 7) /1.00, 0.00, 1.00/
      DATA RGBV(1, 8),RGBV(2, 8),RGBV(3, 8) /1.00, 1.00, 0.00/
      DATA RGBV(1, 9),RGBV(2, 9),RGBV(3, 9) /1.00, 0.50, 0.00/
      DATA RGBV(1,10),RGBV(2,10),RGBV(3,10) /0.75, 0.75, 0.75/
      DATA RGBV(1,11),RGBV(2,11),RGBV(3,11) /0.31, 0.18, 0.31/
      DATA RGBV(1,12),RGBV(2,12),RGBV(3,12) /0.86, 0.58, 0.44/
      DATA RGBV(1,13),RGBV(2,13),RGBV(3,13) /0.65, 0.16, 0.16/
      DATA RGBV(1,14),RGBV(2,14),RGBV(3,14) /0.80, 0.50, 0.20/
      DATA RGBV(1,15),RGBV(2,15),RGBV(3,15) /0.74, 0.56, 0.56/
      DATA RGBV(1,16),RGBV(2,16),RGBV(3,16) /0.68, 0.92, 0.92/
      DATA CUSED/16*0/
C
C  DEFINE THE BYTE SIZE FOR COUNT AND OPTION CODE
C
      DATA BYTE8/8/ , W16/16/
C
C  INITIALIZE THE STATUS FIELD TO ALL OK
C
      STATUS = ALLOK
C
C  GET THE COUNT FIELD NO ERROR CHECKING REQUIRED
C
      CALL MNINST(BYTE8,IOS,STATUS)
      OCOUNT = OPCL
C
C  GET THE OPTION CODE
C
      CALL MNINST(BYTE8,IOS,STATUS)
      OPCODE = OPCL
      IF ( STATUS .NE. ALLOK) RETURN
C
C  SELECT THE PROPER OPTION TO SET
C
      IF (OPCODE .EQ. ICAS) THEN
C
C       ICAS MEANS SET UPPER OR LOWER CASE.
C
        CALL MNINST(BYTE8,IOS,STATUS)
        MCASE = OPCL
C
      ELSE IF (OPCODE .EQ. IINT) THEN
C
C       IINT MEANS SET INTENSITY
C
        CALL MNINST(BYTE8,IOS,STATUS)
        MINTEN = OPCL
C
C       PUT THE INTENSITY VALUE IN THE VALID RANGE
C
        ITMP = MAX(0,MINTEN)
        ITMP = MIN(255,ITMP)
C
C       DO NOT RE-DEFINE COLOR INDICES 0 OR 1.
C
        IF (ITMP .EQ. 204) THEN
          ICNDX = 1
          GO TO 30
        ELSE IF (ITMP .EQ. 0) THEN
          ICNDX = 0
          GO TO 30
        ENDIF
        IF (NUMI .GT. 1) THEN
          DO 10 I=2,NUMI
          IF (INTS(I) .EQ. ITMP) THEN
            ICNDX = I
            GO TO 30
          ENDIF
   10     CONTINUE
        ENDIF
        NUMI = NUMI+1
        INTS(NUMI) = ITMP
        ICNDX = NUMI
        RGB = REAL(ITMP)/255.
        COLVLS(1,NUMI) = RGB
        COLVLS(2,NUMI) = RGB
        COLVLS(3,NUMI) = RGB
        CALL GSCR(1,ICNDX,RGB,RGB,RGB)
   30   CONTINUE
        CALL PLOTIT(0,0,2)
        CURCOL = ICNDX
C
        CALL PLOTIT(0,0,2)
        CALL GSPLCI(ICNDX)
        CALL GSTXCI(ICNDX)
        CALL GSPMCI(ICNDX)
C
      ELSE IF (OPCODE .EQ. IORNT) THEN
C
C       IORNT MEANS SET CHARACTER ORIENTATION.
C       FIRST SKIP OVER A NULL BIT
        CALL MNINST(BYTE8,IOS,STATUS)
C
C       GET THE ORIENTATION VALUE
C
        CALL MNINST(W16,IOS,STATUS)
        MOR = OPCL
C
      ELSE IF (OPCODE .EQ. ISIZE) THEN
C
C       ISIZE MEANS TO SET THE CHARACTER SIZE
C       FIRST SKIP OVER A NULL BIT
        CALL MNINST(BYTE8,IOS,STATUS)
C
C       GET THE CHARACTER SIZE
C
        CALL MNINST(W16,IOS,STATUS)
        MSIZE = OPCL
C
      ELSE IF (OPCODE .EQ. IFONT) THEN
C
C       IFONT MEANS TO SET THE CHARACTER FONT (THE DEFAULT FONT IS
C       ALWAYS USED).
C
        CALL MNINST(BYTE8,IOS,STATUS)
        MFONT = 1
C
      ELSE IF (OPCODE .EQ. IPAT) THEN
C
C       IPAT MEANS TO SET THE DASH PATTERN
C       FIRST SKIP OVER A NULL BIT
        CALL MNINST(BYTE8,IOS,STATUS)
C
C       GET THE DASH PATTERN AND SET IT
C
        CALL MNINST(W16,IOS,STATUS)
        MDASH = OPCL
        CALL PLOTIT(0,0,2)
        CALL PATSET(MDASH)
C
      ELSE IF (OPCODE .EQ. ISSIZ) THEN
C
C       ISSIZ MEANS TO SET THE SPOT SIZE
C       FIRST SKIP OVER A NULL BIT
        CALL MNINST(BYTE8,IOS,STATUS)
C
C       GET THE SPOT SIZE CONVERT TO SCALE FACTOR AND SEND TO LINE WIDTH
C       PROCESSOR
C
        CALL MNINST(W16,IOS,STATUS)
        LINWTH = MAX(1.,FLOAT(OPCL) / 8.0)
        CALL PLOTIT(0,0,2)
        CALL GSLWSC(LINWTH)
C
      ELSE IF (OPCODE .EQ. ICENT) THEN
C
C       ICENT MEANS GET THE CHARACTER CENTERING OPTION
C       GET THE CENTERING OPTION
C
        CALL MNINST(BYTE8,IOS,STATUS)
        MCENT = OPCL
C
      ELSE IF (OPCODE .EQ.ICOLR) THEN
C
C       ICOLR MEANS SET THE COLOR OPTION
C
C       The color option has three bytes of information.
C
        CALL MNINST(BYTE8,IOS,STATUS)
        MCOLOR(1) = OPCL
        CALL MNINST(BYTE8,IOS,STATUS)
        MCOLOR(2) = OPCL
        CALL MNINST(BYTE8,IOS,STATUS)
        MCOLOR(3) = OPCL
C
C  First check to see if the three bytes describe a color index in the
C  range 000 to 999.  If so, set the current color index to that value.
C
        DO 110 I=1,3
C
C  Convert blanks to zeros.
C
          IF (CHAR(MCOLOR(I)) .EQ. ' ') MCOLOR(I) = ICHAR('0')
          DO 120 J=1,10
            IF (CHAR(MCOLOR(I)) .EQ. CINT(J)) THEN
              CINDX(I:I) = CHAR(MCOLOR(I))
              GO TO 110
            ENDIF
  120     CONTINUE
          GO TO 150
  110   CONTINUE
        READ(CINDX,501) CURCOL
        GO TO 160
C
C  Check if the color matches a pre-defined one.
C
  150   CONTINUE
        DO 130 I=1,3
          CINDX(I:I) = CHAR(MCOLOR(I))
  130   CONTINUE
        DO 140 I=1,16
          IF (CINDX .EQ. CVAL(I)) THEN
C
C  Check if this color has been previously used.
C
            IF (CUSED(I) .EQ. 0) THEN
C
C             Not used, define new color index.
C
              NUMI = NUMI+1
              CUSED(I) = NUMI
              COLVLS(1,NUMI) = RGBV(1,I)
              COLVLS(2,NUMI) = RGBV(2,I)
              COLVLS(3,NUMI) = RGBV(3,I)
              CALL GSCR(1,NUMI,RGBV(1,I),RGBV(2,I),RGBV(3,I))
              CURCOL = NUMI
            ELSE
              CURCOL = CUSED(I)
            ENDIF
            GO TO 160
          ENDIF
  140   CONTINUE
C
C  No match, default to foreground color.
C
        CURCOL = 1
C
C       SET COLORS
C
  160   CONTINUE
        CALL PLOTIT(0,0,2)
        CALL GSTXCI(CURCOL)
        CALL GSPLCI(CURCOL)
        CALL GSPMCI(CURCOL)
C
      ELSE
C
C       UNDEFINED OPCODE
C
        WRITE(6,'('' Unknown option in OPTN call'')')
C
C       READ IN REMAINDER OF OPCODES OPERANDS
C
        NSKIP = (OCOUNT-1)/2
        CALL MNINST(BYTE8,IOS,STATUS)
        IF (NSKIP.EQ.0) GO TO 500
        DO 200 K=1,NSKIP
                CALL MNINST(W16,IOS,STATUS)
                IF (STATUS.NE.ALLOK) RETURN
  200   CONTINUE
C
  500   CONTINUE
      END IF
C
      RETURN
C
  501 FORMAT(I3)
C
      END
