C
C $Id: basic01f.f,v 1.4 1995-03-31 21:31:24 haley Exp $
C
C***********************************************************************
C                                                                      *
C                            Copyright (C)  1995                       *
C                 University Corporation for Atmospheric Research      *
C                            All Rights Reserved                       *
C                                                                      *
C***********************************************************************
C
C      File:            basic01f.f
C
C      Author:          Tim Scheitlin (converted by Ed Stautler)
C                       National Center for Atmospheric Research
C                       PO 3000, Boulder, Colorado
C
C      Date:            Mon Mar 20 10:43:42 MST 1995
C
C      Description:     This example demonstrates how to draw a contour
C                       plot using mostly defaults.  Note: no data is
C                       used in this example, so the output appears
C                       only as a bounding box with tickmarks.
C 
C                       The mininum set of steps needed for creating
C                       any plot involve the following:
C
C                       1. Initialize the graphics libraries
C                       2. Choose the type of output 
C                       3. Create a plot object
C                       4. Draw the plot
C                       5. Call frame
C                       6. Clean up memory
C
      program basic01f
      implicit none

      external NhlFAppLayerClass
      external NhlFXWorkstationLayerClass
      external NhlFContourLayerClass

      integer appid,wks,con1,rlist,ierr
C
C ##########
C # STEP 1 #
C ##########
C Initialize the graphics libraries and create a resource list that
C is normally used to assign name/value pairs within objects.  Then
C clear (empty) this list, and create an application object.  This
C object manages multiple resource databases used by seperate objects.  
C
C The first argument, appid, is a variable that identifies the object.
C The second argument, '"basic01"', sets the name of the object being
C created.
C The third argument, "NhlFAppLayerClass", identifies the type or class 
C of the created object. 
C The fourth argument, "0", specifies the id of the objects parent.
C In this case, the object has no parent, so the value 0 is used. 
C The fifth argument, "rlist", is the resource list modifiers to be used
C when creating the object.  In this example, no modifications are made
C to default values.
C The sixth argument, "ierr", is used to return an error code.
C
C These steps are always required when writing an HLU application
C program.
C
      call NhlFInitialize
      call NhlFRLCreate(rlist,'SETRL')

      call NhlFRLClear(rlist)
      call NhlFCreate(appid,"basic01",NhlFAppLayerClass,0,rlist,ierr)
C
C ##########
C # STEP 2 #
C ##########
C Choose the type of output you want to create.  You may write your
C output to an NCAR Computer Graphics Metafile (NCGM) and view it
C later using the NCAR Graphics utilities ctrans or idt.  You may also
C write your output directly into a window of a workstation running
C the X Window system (as demonstrated in this example), or you can
C write your ouput into  a PostScript file.  
C
C The first argument, wks, is a variable that identifies the object.
C The second argument, '"wks"', sets the name of the object being
C created. The third argument, "NhlFXWorkstationLayerClass",
C identifies the type  or class of the object to create.  In this case
C an X workstation. The fourth argument, "0", specifies the id of the
C objects parent. In this case, the object has no parent, so the
C constant 0 is used.  The fifth argument, "rlist", is the resource
C list modifiers to be used when creating the object.  In this
C example, no modifications are made to default values.
C The sixth argument, "ierr", is used to return an error code.
C
      call NhlFRLClear(rlist)
      call NhlFCreate(wks,"wks",NhlFXWorkstationLayerClass,0,
     1      rlist,ierr)
C
C ##########
C # STEP 3 #
C ##########
C Create a plot object.  In this example, we will create a contour plot,
C but we could have just as easily created any other type of plot such as
C an Xy plot, or a Map plot.
C
C The first argument, con1, is a variable that identifies the object.
C The second create call argument, '"con1"', sets the name of the object.
C This is an arbitrary name and does not have to match the variable
C object identifier used in the first parameter.
C The third argument, "NhlFContourLayerClass", identifies the type or
C class of the object to create.  In this case, the type is a contour
C plot.  The third argument, "wks", specifies the id of the object's
C parent.  By  specifying the id of the X workstation created earlier,
C the plot will be drawn into an X window.
C The fifth argument, "rlist", is the resource list modifiers to be used
C when creating the object.  In this example, no modifications are
C made to default values.
C The sixth argument, "ierr", is used to return an error code.
C
      call NhlFRLClear(rlist)
      call NhlFCreate(con1,"con1",NhlFContourLayerClass,wks,
     1      rlist,ierr)
C
C ##########
C # STEP 4 #
C ##########
C This step draws the plot into the X workstation window.  The first
C argument  to the draw function is the variable name of the object
C that you want to draw.  The second argument is for returning error
C codes.
C
C
      call NhlFDraw(con1,ierr)
C
C ##########
C # STEP 5 #
C ##########
C The frame call updates and then clears the workstation.
C
      call NhlFFrame(wks,ierr)
C
C ##########
C # STEP 6 #
C ##########
C This is the final step used for cleanup.  The delete
C function deletes variables from the NCL and frees the
C symbol name from the symbol table.  Deleting a parent object
C automatically deletes all of its children.  The close function
C is used to tell the HLU library that the programmer is done 
C using it, and to free up any memory that it can. 
C
      call NhlFDestroy(con1,ierr)
      call NhlFClose

      stop
      end
