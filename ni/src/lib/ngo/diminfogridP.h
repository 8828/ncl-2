/*
 *      $Id: diminfogridP.h,v 1.4 1999-11-03 20:29:29 dbrown Exp $
 */
/************************************************************************
*									*
*			     Copyright (C)  1996			*
*	     University Corporation for Atmospheric Research		*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		diminfogridP.h
 *
 *	Author:		David I. Brown
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Mon Feb 10 14:22:58 MST 1997
 *
 *	Description:	
 */
#ifndef	_NG_DIMINFOGRIDP_H_
#define	_NG_DIMINFOGRIDP_H_

#include <ncarg/ngo/goP.h>

#include <ncarg/ngo/diminfogrid.h>

 
#define DEBUG_DIM_INFO_GRID 0
#define BUFINC 256

/*
 * Any field in the public structure NgDimInfoGrid, defined in
 * diminfogrid.h, must appear in the same order at the beginning
 * of the NgDimInfoGridRec definition. 
 */

typedef struct _NgDimInfoGridRec 
{
            /* public fields - exported as NgDimInfoGrid */
        
        Widget			grid;
        NhlBoolean		headline_on;
        NhlBoolean		highlight_on;
        Dimension		height;
        
            /* private fields */
	NgGO			go;
        NrmQuark		qfileref;
 	NclApiVarInfoRec	*vinfo;
        int			cwidths[32];
	NhlBoolean		creating;
} NgDimInfoGridRec;

#endif	/* _NG_DIMINFOGRIDP_H_ */
