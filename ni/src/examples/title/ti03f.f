CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C                                                                      C
C                Copyright (C) 1995                                    C
C        University Corporation for Atmospheric Research               C
C                all rights reserved                                   C
C                                                                      C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
C      File:           ti03f.f
C
C      Author:         Bob Lackman (converted by Ed Stautler)
C          National Center for Atmospheric Research
C          PO 3000, Boulder, Colorado
C
C      Date:           Fri Jan 06 18:31:18 mdt 1995
C
C      Description:    Demonstrates Title Object resource changes.
C

      external NhlFAppLayerClass
      external NhlFTitleLayerClass
      external NhlFXWorkstationLayerClass
      external NhlFNcgmWorkstationLayerClass

      integer appid, wid, pid
      integer rlist, ierr
      integer NCGM
C
C Default is to create an X workstation.
C
      NCGM=0
C
C Initialize the high level utility library
C
      call NhlFInitialize
C
C Create an application context. Set the app dir to the current
C directory so the application looks for a resource file in the
C working directory. In this example the resource file supplies
C the plot title only.
C
      call NhlFRLCreate(rlist,'setrl')
      call NhlFRLClear(rlist)
      call NhlFRLSetstring(rlist,'appDefaultParent','True',ierr)
      call NhlFRLSetstring(rlist,'appUsrDir','./',ierr)
      call NhlFCreate(appid,'ti03',NhlFAppLayerClass,
     $       0,rlist,ierr)

      if (NCGM.eq.1) then
C
C Create an NCGM workstation object.
C
         call NhlFRLClear(rlist)
         call NhlFRLSetstring(rlist,'wkMetaName','./ti03f.ncgm',ierr)
         call NhlFCreate(wid,'ti03Work',NhlFNcgmWorkstationLayerClass,0,
     $        rlist,ierr)
      else
C
C Create an xworkstation object.
C
         call NhlFRLClear(rlist)
         call NhlFRLSetstring(rlist,'wkPause','True',ierr)
         call NhlFCreate(wid,'ti03Work',NhlFXWorkstationLayerClass,
     $        0,rlist,ierr)
      endif
C
C Specify the viewport extent of the object.
C

      call NhlFRLClear(rlist)
      call NhlFRLSetfloat(rlist,'vpXF',.2,ierr)
      call NhlFRLSetfloat(rlist,'vpYF',.8,ierr)
      call NhlFRLSetfloat(rlist,'vpWidthF',.6,ierr)
      call NhlFRLSetfloat(rlist,'vpHeightF',.6,ierr)

      call NhlFCreate(pid,'Titles',
     $       NhlFTitleLayerClass,wid,rlist,ierr)

      call NhlFDraw(pid,ierr)
      call NhlFFrame(wid,ierr)
      call NhlFDestroy(pid,ierr)
      call NhlFDestroy(wid,ierr)
      call NhlFDestroy(appid,ierr)
      call NhlFClose

      stop
      end

