C
C	$Id: vvinit.f,v 1.1 1992-12-02 23:43:19 dbrown Exp $
C
C-----------------------------------------------------------------------
C
      SUBROUTINE VVINIT (U,LU,V,LV,P,LP,M,N,IWK,IC,RWK,RC)
C
C Argument dimensions.
C
      DIMENSION       U(LU,N)    ,V(LV,N)    ,P(LP,N)
      DIMENSION       IWK(IC)    ,RWK(RC)
C                        P
C                          A two-dimensional array of scalar data 
C                          representing an independent variable (e.g.
C                          temperature or pressure) associated with 
C                          the velocity vector field and realized as 
C                          variations in the color of the velocity 
C                          vectors. The colors to be used and the
C                          threshold values dividing the data may be
C                          set using the internal parameter setting
C                          routines. Otherwise a default scheme 
C                          linearly divides the data into 16 groups
C                          represented by 16 colors.
C
C                        PV
C                          The first dimension of P in the calling 
C                          program. Must be >= M.
C
C *********************************************************************
C
C Declare the VV common blocks.
C
      PARAMETER (IPLVLS = 64)
C
C Integer and real common block variables
C
C
      COMMON /VVCOM/
     +                IUD1       ,IVD1       ,IPD1       ,IXDM       ,
     +                IYDN       ,VLOM       ,VHIM       ,ISET       ,
     +                VMXL       ,VFRC       ,IXIN       ,IYIN       ,
     +                ISVF       ,UUSV       ,UVSV       ,
     +                UPSV       ,IMSK       ,ICPM       ,UVPS       ,
     +                UVPL       ,UVPR       ,UVPB       ,UVPT       ,
     +                UWDL       ,UWDR       ,UWDB       ,UWDT       ,
     +                UXC1       ,UXCM       ,UYC1       ,UYCM       ,
     +                NLVL       ,IPAI       ,ICTV       ,WDLV       ,
     +                UVMN       ,UVMX       ,PMIN       ,PMAX       ,
     +                ISPC       ,ITHN       ,IPLR       ,
     +                IVPO       ,ILBL       ,IDPF       ,IMSG       ,
     +                ICLR(IPLVLS)           ,TVLU(IPLVLS)
C
C Arrow size/shape parameters
C
	COMMON / VVARO /
     +                HDSZ       ,HINF       ,HANG       ,
     +	              HSIN       ,HCOS       ,FAMN       ,FAMX

C
C Text related parameters
C
	COMMON /VVTXP /
     +                FCWM    ,ICSZ    ,
     +                FMNS    ,FMNX    ,FMNY    ,IMNP    ,IMNC  ,
     +                FMXS    ,FMXX    ,FMXY    ,IMXP    ,IMXC  ,
     +                FZFS    ,FZFX    ,FZFY    ,IZFP    ,IZFC  ,
     +                FILS    ,FILX    ,FILY    ,IILP     IILC  ,
     +                FLBS    ,ILBC

C
C Character variable declartions
C
      CHARACTER*160 CSTR
      PARAMETER (IPCHSZ=36)
      CHARACTER*(IPCHSZ)  CMNT,CMXT,CZFT,CLBT,CILT
C
C Text string parameters
C
      COMMON /VVCHAR/ CSTR,CMNT,CMXT,CZFT,CLBT,CILT
C
      SAVE /VVCOM/, /VVARO/, /VVTXP/, /VVCHAR/
C
C The mapping common block: made available to user mapping routines
C
      COMMON /VVMAP/
     +                IMAP       ,
     +                XVPL       ,XVPR       ,YVPB       ,YVPT       ,
     +                WXMN       ,WXMX       ,WYMN       ,WYMX       ,
     +                XLOV       ,XHIV       ,YLOV       ,YHIV       ,
     +                SXDC       ,SYDC       ,NXCT       ,NYCT       ,
     +                RLEN       ,LNLG       ,INVX       ,INVY       ,
     +                ITRT       ,IWCT       ,FW2W       ,FH2H       ,
     +                DVMN       ,DVMX       ,RBIG       ,IBIG
C
      SAVE /VVMAP/
C
C Math constants
C
      PARAMETER (PDTOR  = 0.017453292519943,
     +           PRTOD  = 57.2957795130823,
     +           P1XPI  = 3.14159265358979,
     +           P2XPI  = 6.28318530717959,
     +           P1D2PI = 1.57079632679489,
     +           P5D2PI = 7.85398163397448) 
C
C --------------------------------------------------------------------
C
C Force the block data routine, which sets default variables, to load. 
C
      EXTERNAL        VELDAT
C
C Write the array sizes into the common block
C 
      IUD1=LU
      IVD1=LV
      IPD1=LP
C
C Error if M > LU or M > LV?
C
      IXDM=MIN(M,LU,LV)
      IYDN=N
C
C Initialize and transfer some arguments to local variables.
C
c      write(*,*) "dib's prototype version"
      IBIG = I1MACH(9)
      RBIG = R1MACH(2)
C
C Decide what the range of values in X and Y should be.
C
      IF (UXC1.EQ.UXCM) THEN
        XLOV=1.
        XHIV=REAL(IXDM)
      ELSE
        XLOV=UXC1
        XHIV=UXCM
      END IF
C
      IF (UYC1.EQ.UYCM) THEN
        YLOV=1.
        YHIV=REAL(IYDN)
      ELSE
        YLOV=UYC1
        YHIV=UYCM
      END IF
C
      IXIN = MAX(IXIN,1)
      IYIN = MAX(IYIN,1)
C
      NXCT = IXDM/IXIN
      NYCT = IYDN/IYIN
C
C If the user has done a SET call, retrieve the arguments; if he hasn't
C done a SET call, do it for him.
C
      IF (ISET .EQ .0) THEN
C
        CALL GETSET (XVPL,XVPR,YVPB,YVPT,WXMN,WXMX,WYMN,WYMX,LNLG)
C
      ELSE
C
        LNLG=1
C
        IF (UWDL.EQ.UWDR) THEN
          WXMN=XLOV
          WXMX=XHIV
        ELSE
          WXMN=UWDL
          WXMX=UWDR
        END IF
C
        IF (UWDB.EQ.UWDT) THEN
          WYMN=YLOV
          WYMX=YHIV
        ELSE
          WYMN=UWDB
          WYMX=UWDT
        END IF
C
C Determine the viewport based on the setting of the viewport
C shape and viewport extent parameters
C
        IF (UVPS.LT.0.) THEN
          AR=ABS(UVPS)
        ELSE IF (UVPS.EQ.0.) THEN
          AR=(UVPR-UVPL)/(UVPT-UVPB)
        ELSE IF (UVPS.LE.1.) THEN
          AR=ABS((WXMX-WXMN)/(WYMX-WYMN))
          IF (MIN(AR,1./AR).LT.UVPS) AR=(UVPR-UVPL)/(UVPT-UVPB)
        ELSE
          AR=ABS((WXMX-WXMN)/(WYMX-WYMN))
          IF (MAX(AR,1./AR).GT.UVPS) AR=1.
        END IF
C
        IF (AR.LT.(UVPR-UVPL)/(UVPT-UVPB)) THEN
          XVPL=.5*(UVPL+UVPR)-.5*(UVPT-UVPB)*AR
          XVPR=.5*(UVPL+UVPR)+.5*(UVPT-UVPB)*AR
          YVPB=UVPB
          YVPT=UVPT
        ELSE
          XVPL=UVPL
          XVPR=UVPR
          YVPB=.5*(UVPB+UVPT)-.5*(UVPR-UVPL)/AR
          YVPT=.5*(UVPB+UVPT)+.5*(UVPR-UVPL)/AR
        END IF
C
        CALL SET (XVPL,XVPR,YVPB,YVPT,WXMN,WXMX,WYMN,WYMX,LNLG)
C
      END IF
C
C Calculate fraction of VP width to fractional size factor.
C Calculate fraction of VP height to fractional size factor.
C These are for convenience.
C
      FW2W = XVPR - XVPL
      FH2H = YVPT - YVPB
C
C Swap window rectangle if it is inverted, but keep track
C This makes it easier to exclude out-of-bounds points in the
C projection mapping routines
C
      INVX=0
      INVY=0
      IF (WXMN .GT. WXMX) THEN
         T=WXMN
         WXMN=WXMX
         WXMX=T
         INVX=1
      END IF
      IF (WYMN .GT. WYMX) THEN
         T=WYMN
         WYMN=WYMX
         WYMX=T
         INVY=1
      END IF
C
C Calculate a default value for the maximum vector length in
C fractional coordinates. 
C
      IXIN = MAX(IXIN, 1)
      IYIN = MAX(IYIN, 1)
      I=MAX(IXDM/IXIN, 1)
      J=MAX(IYDN/IYIN, 1)
C
      TX=(XVPR-XVPL)/FLOAT(I)
      TY=(YVPT-YVPB)/FLOAT(J)
      DVMX=SQRT((TX*TX+TY*TY)/2.0)
C
C Determine the equivalent maximum length in the user coordinate
C system. Note that if the X axis is non-uniform or if the Y axis
C has a different scaling from the X-Axis, this value may not be 
C meaningful.
C
      RLEN=DVMX*(WXMX-WXMN)/(XVPR-XVPL)
C
C Find the maximum and minimum magnitude vectors.
C If there are no special values use simpler code for efficiency.
C
      UVMN=RBIG
      UVMX=-RBIG
      IF (ISVF .LE. 0) THEN
C
         DO 111 J=1,IYDN,IYIN
            DO 110 I=1,IXDM,IXIN
               TU=U(I,J)
               TV=V(I,J)
               IF (IPLR) THEN
                  UVMN = MIN(UVMN, TU)
                  UVMX = MAX(UVMX, TU)
               ELSE
                  UVMN = MIN(UVMN, TU*TU+TV*TV)
                  UVMX = MAX(UVMX, TU*TU+TV*TV)
               END IF
 110        CONTINUE
 111     CONTINUE
C
      ELSE
C
         DO 116 J=1,IYDN,IYIN
            DO 115 I=1,IXDM,IXIN
               TU=U(I,J)
               TV=V(I,J)
C
               IF (ISVF .EQ. 1) THEN
                  IF (TU .EQ. UUSV) GO TO 115
               ELSE IF (ISVF .EQ. 2) THEN
                  IF (TV .EQ. UVSV) GO TO 115
               ELSE IF (ISVF .EQ. 3) THEN
                  IF (TU .EQ. UUSV .OR. TV .EQ. UVSV) THEN
                     GO TO 115
                  END IF
               ELSE IF (ISVF .EQ. 4) THEN
                  IF (TU .EQ. UUSV .AND. TV .EQ. UVSV) THEN
                     GO TO 115
                  END IF
               END IF
C
               IF (IPLR) THEN
                  UVMN = MIN(UVMN, TU)
                  UVMX = MAX(UVMX, TU)
               ELSE
                  UVMN = MIN(UVMN, TU*TU+TV*TV)
                  UVMX = MAX(UVMX, TU*TU+TV*TV)
               END IF
C
 115        CONTINUE
 116     CONTINUE
C
      END IF
C
      IF (IPLR .LE. 0) THEN
         UVMX = SQRT(UVMX)
         UVMN = SQRT(UVMN)
      END IF
C
C Decide how to use the scalar array.
C If there would otherwise be no selection of values but NLVL is 0
C use 16 divisions.
C
      IF (ICTV .LT. 0) THEN
         PMIN=UVMN
         PMAX=UVMX
      ELSE IF (ICTV .GT. 0) THEN
C
C Check for error condition
C
         IF (IPD1 .LT. IXDM) WRITE (*,*) 
     +        'VVINIT - Scalar array too small'
C
C Find the max and min scalar values when necessary
C
         IF (ICTV .GT. 1 .OR. PMIN .EQ. PMAX) THEN
            PMIN=RBIG
            PMAX=-RBIG
            DO 121 J=1,IYDN,IYIN
               DO 120 I=1,IXDM,IXIN
                  IF (ISPC .GE. 0 .AND. P(I,J) .EQ. UPSV) GO TO 120  
                  PMIN = MIN(PMIN, P(I,J))
                  PMAX = MAX(PMAX, P(I,J))
 120           CONTINUE
 121        CONTINUE
         END IF
C
      END IF
C
C Determine the threshold values
C
      IF (ABS(ICTV) .GT. 1 .OR. NLVL .LE. 0) THEN
C
         IF (ABS(ICTV) .GT. 1) THEN
            NLVL = ABS(ICTV)
         ELSE
            NLVL = 16
         END IF
C
         TV=(PMAX-PMIN)/NLVL
C
         DO 125 I=1,NLVL,1
            TVLU(I)=PMIN+REAL(I)*TV
 125     CONTINUE
C
      END IF
C
C Calculate the sine and cosine of the arrow head half angle
C
      HSIN = SIN(PDTOR*HANG)
      HCOS = COS(PDTOR*HANG)
C
C Done.
C
      RETURN
C
      END
