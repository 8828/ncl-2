/*
 *      $Id: ScalarFieldFloatP.h,v 1.6 2002-07-02 01:26:40 dbrown Exp $
 */
/************************************************************************
*									*
*			     Copyright (C)  1993			*
*	     University Corporation for Atmospheric Research		*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		ScalarFieldFloatP.h
 *
 *	Author:		David I. Brown
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Wed Apr  6 17:53:29 MDT 1994
 *
 *	Description:	This file contains all the declarations for the
 *			ScalarFieldFloat object.  This object is a private
 *			object to be used internally by DataComm sub-classes
 *			and by the ScalarField object so there is no
 *			public header file.
 */
#ifndef _NScalarFieldFloatP_h
#define _NScalarFieldFloatP_h
#include <ncarg/hlu/BaseP.h>
#include <ncarg/hlu/ScalarField.h>


typedef struct _NhlScalarFieldFloatClassRec 
				*NhlScalarFieldFloatClass;
typedef struct _NhlScalarFieldFloatLayerRec *NhlScalarFieldFloatLayer;

extern NhlClass NhlscalarFieldFloatClass;


#define _NhlsfDARR_CHANGED 1
#define _NhlsfXARR_CHANGED 2
#define _NhlsfYARR_CHANGED 4

typedef struct _NhlScalarFieldFloatLayerPart{

	/* all fields are private and set directly by the converter */

	NhlGenArray	d_arr;
	NhlGenArray	x_arr;
	NhlGenArray	y_arr;
	NhlGridType     grid_type;

	NhlBoolean	missing_value_set;
	float		missing_value;
	float		data_min;
	float		data_max;
        
	float		x_start;
	float		x_end;
	float		y_start;
	float		y_end;
        
        int		ix_start;
        int		ix_end;
        int		iy_start;
        int		iy_end;

	int		begin;
	int		fast_dim;
	int		fast_len;
	int		slow_len;
	int		changed;
} NhlScalarFieldFloatLayerPart;

typedef struct _NhlScalarFieldFloatLayerRec{
	NhlObjLayerPart			base;
	NhlScalarFieldFloatLayerPart	sfieldfloat;
} NhlScalarFieldFloatLayerRec;

typedef struct _NhlScalarFieldFloatClassPart{
	int	foo;
} NhlScalarFieldFloatClassPart;

typedef struct _NhlScalarFieldFloatClassRec{
	NhlObjClassPart			base_class;
	NhlScalarFieldFloatClassPart	sfieldfloat_class;
} NhlScalarFieldFloatClassRec;

extern NhlScalarFieldFloatClassRec NhlscalarFieldFloatClassRec;

#endif	/* _NScalarFieldFloatP_h */
