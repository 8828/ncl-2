/*
 *      $Id: datasourcegrid.h,v 1.4 2000-03-21 02:35:37 dbrown Exp $
 */
/************************************************************************
*									*
*			     Copyright (C)  1996			*
*	     University Corporation for Atmospheric Research		*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		datasourcegrid.h
 *
 *	Author:		David I. Brown
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Sun Jun 22 14:31:22 MDT 1997
 *
 *	Description:	
 */
#ifndef	_NG_DATASOURCEGRID_H
#define	_NG_DATASOURCEGRID_H

#include <ncarg/ngo/go.h>
#include <ncarg/ngo/browse.h>
#include <ncarg/ngo/dataprofile.h>

/*
 * Public api
 */

typedef struct _NgDataSourceGrid
{
        Widget			grid;
        NhlBoolean		headline_on;
        Dimension		height;
} NgDataSourceGrid;


NgDataSourceGrid *NgCreateDataSourceGrid
(
	int			go_id,
        Widget			parent,
        NrmQuark		qname,
        NgDataProfile		data_profile
        );

NhlErrorTypes NgUpdateDataSourceGrid
(
        NgDataSourceGrid	*data_source_grid,
        NrmQuark		qname,
        NgDataProfile		data_profile
        );

void NgDestroyDataSourceGrid
(
        NgDataSourceGrid		*data_source_grid
        );

void NgDeactivateDataSourceGrid
(
        NgDataSourceGrid		*data_source_grid
        );
        

#endif	/* _NG_DATASOURCEGRID_H */
