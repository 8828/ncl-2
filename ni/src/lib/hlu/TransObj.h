/*
 *      $Id: TransObj.h,v 1.14 2002-03-18 21:20:07 dbrown Exp $
 */
/************************************************************************
*									*
*			     Copyright (C)  1992			*
*	     University Corporation for Atmospheric Research		*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		TransObj.h
 *
 *	Author:		Ethan Alpert
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Fri Oct 16 10:48:21 MDT 1992
 *
 *	Description:	This is the public header file for the TransObj class.
 *			The TransObjClass is responsible for managing 
 *			transformations DATA==>VIEWPORT definitions.
 */
#ifndef _NTransObj_h
#define  _NTransObj_h

#include <ncarg/hlu/Base.h>

#define NhlTAxisType "AxisType"

#define NhlNtrXMinF		"trXMinF"
#define NhlCtrXMinF		"TrXMinF"
#define NhlNtrXMaxF		"trXMaxF"
#define NhlCtrXMaxF		"TrXMaxF"
#define NhlNtrXReverse		"trXReverse"
#define NhlCtrXReverse		"TrXReverse"

#define NhlNtrYMinF		"trYMinF"
#define NhlCtrYMinF		"TrYMinF"
#define NhlNtrYMaxF		"trYMaxF"
#define NhlCtrYMaxF		"TrYMaxF"
#define NhlNtrYReverse		"trYReverse"
#define NhlCtrYReverse		"TrYReverse"

#define NhlNtrOutOfRangeF	"trOutOfRangeF"
#define NhlCtrOutOfRangeF	"TrOutOfRangeF"
#define NhlNtrResolutionF	"trResolutionF"
#define NhlCtrResolutionF	"TrResolutionF"

#define NhlNtrLineInterpolationOn 	"trLineInterpolationOn"
#define NhlCtrLineInterpolationOn 	"TrLineInterpolationOn"

/*
 * both Irregular and Curvilinear define these array resources
 */

#define NhlNtrXCoordPoints	"trXCoordPoints"
#define NhlCtrXCoordPoints	"TrXCoordPoints"
#define NhlNtrYCoordPoints	"trYCoordPoints"
#define NhlCtrYCoordPoints	"TrYCoordPoints"

extern NhlClass NhltransObjClass;


#endif  /*_NTransObj_h*/
