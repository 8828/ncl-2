C
C $Id: mpiaty.f,v 1.2 1998-04-30 22:43:53 kennison Exp $
C
      INTEGER FUNCTION MPIATY (IAIN)
C
        PARAMETER (MNAI=2000)
C
C The value of "MPIATY(IAIN)" is the area type for the area whose area
C identifier is IAIN.
C
        COMMON /MAPCMX/ IATY(MNAI),ISCI(MNAI),IPAR(MNAI)
        SAVE   /MAPCMX/
C
        MPIATY=0
        IF (IAIN.GE.1.AND.IAIN.LE.MNAI) THEN
          MPIATY=IATY(IAIN)
        END IF
        RETURN
      END
