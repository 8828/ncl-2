C
C $Id: lbsetr.f,v 1.4 1994-03-17 20:19:54 kennison Exp $
C
      SUBROUTINE LBSETR (WHCH,RVAL)
C
        CHARACTER*(*) WHCH
C
C This subroutine is called to set the real value of a specified
C parameter.
C
C WHCH is the name of the parameter whose value is to be set.
C
C RVAL is a real variable containing the new value of the parameter.
C
C
C Declare the common block where internal parameters are stored.
C
        COMMON /LBCOMN/ ICBL,ICFL,ICLB,WOBL,WOFL,WOLB
        SAVE   /LBCOMN/
C
C Declare the block data routine external to force it to load.
C
        EXTERNAL LBBLDA
C
C Define a character temporary for use in forming error messages.
C
        CHARACTER*39 CTMP
C
C Check for an uncleared prior error.
C
        IF (ICFELL('LBSETR - UNCLEARED PRIOR ERROR',1).NE.0) RETURN
C
C Check for a parameter name that is too short.
C
        IF (LEN(WHCH).LT.3) THEN
          CTMP(1:36)='LBSETR - PARAMETER NAME TOO SHORT - '
          CTMP(37:36+LEN(WHCH))=WHCH
          CALL SETER (CTMP(1:36+LEN(WHCH)),2,1)
          RETURN
        END IF
C
C Set the appropriate parameter value.
C
        IF      (WHCH(1:3).EQ.'CBL'.OR.WHCH(1:3).EQ.'cbl') THEN
          ICBL=MAX(-1,INT(RVAL))
        ELSE IF (WHCH(1:3).EQ.'CFL'.OR.WHCH(1:3).EQ.'cfl') THEN
          ICFL=MAX(-1,INT(RVAL))
        ELSE IF (WHCH(1:3).EQ.'CLB'.OR.WHCH(1:3).EQ.'clb') THEN
          ICLB=MAX(-1,INT(RVAL))
        ELSE IF (WHCH(1:3).EQ.'WBL'.OR.WHCH(1:3).EQ.'wbl') THEN
          WOBL=MAX(0.,RVAL)
        ELSE IF (WHCH(1:3).EQ.'WFL'.OR.WHCH(1:3).EQ.'wfl') THEN
          WOFL=MAX(0.,RVAL)
        ELSE IF (WHCH(1:3).EQ.'WLB'.OR.WHCH(1:3).EQ.'wlb') THEN
          WOLB=MAX(0.,RVAL)
        ELSE
          CTMP(1:36)='LBSETR - PARAMETER NAME NOT KNOWN - '
          CTMP(37:39)=WHCH(1:3)
          CALL SETER (CTMP(1:39),3,1)
          RETURN
        END IF
C
C Done.
C
        RETURN
C
      END
