/*
 *      $Id: shapeinfogrid.c,v 1.21 2000-01-21 05:18:53 dbrown Exp $
 */
/************************************************************************
*									*
*			     Copyright (C)  1996			*
*	     University Corporation for Atmospheric Research		*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		shapeinfogrid.c
 *
 *	Author:		David I. Brown
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Thu May 22 19:15:36 MDT 1997
 *
 *	Description:	
 */

#include <ncarg/ngo/shapeinfogridP.h>
#include <ncarg/ngo/xutil.h>
#include <ncarg/ngo/stringutil.h>
#include <float.h>

#include <Xm/Xm.h>
#include <Xm/Protocols.h>
#include  <Xm/Text.h>
#include  <ncarg/ngo/Grid.h>

static char *Buffer;
static int  Buflen;
static Pixel Foreground,Background,Right_Border_Color,Left_Border_Color;
static NhlString Unnamed = "<unnamed>";

static char *
ColumnWidths
(
	NgShapeInfoGridRec *sirp
)
{
	int	i;
        NclApiVarInfoRec *vinfo = sirp->vinfo;
	float	mult = sirp->go->go.x->avg_font_width_mult;
        char	sizestr[10];

        Buffer[0] = '\0';
	for (i=0; i <= vinfo->n_dims+1; i++) {
                sprintf(sizestr,"%dc ",(int)(mult * sirp->cwidths[i]));
                if (strlen(sizestr) + strlen(Buffer) + 1> Buflen) {
                        Buffer = NhlRealloc(Buffer,Buflen+BUFINC);
                }
		strcat(Buffer,sizestr);
	}
        Buffer[strlen(Buffer)-1] = '\0';
#if DEBUG_SHAPE_INFO_GRID      
        fprintf(stderr,"%s\n",Buffer);
#endif        
        return Buffer;
}

static char *
ShapeText
(
	NgShapeInfoGridRec *sirp
)
{
        sprintf(Buffer,"%dD",sirp->shape);
#if 0        
        sirp->cwidths[0] =
                MAX(sirp->cwidths[0],strlen(Buffer));
#endif        
#if DEBUG_SHAPE_INFO_GRID      
        fprintf(stderr,"%s\n",Buffer);
#endif        
        return Buffer;
}

static char *
TotalText
(
	NgShapeInfoGridRec *sirp
)
{
        int len;
        
        NclApiVarInfoRec *vinfo = sirp->vinfo;
        sprintf(Buffer,"%d of %d",sirp->cur_size,sirp->total_size);
        len = strlen(Buffer);
        sirp->cwidths[vinfo->n_dims+1] =
                MAX(sirp->cwidths[vinfo->n_dims+1],len);
        
#if DEBUG_SHAPE_INFO_GRID      
        fprintf(stderr,"%s\n",Buffer);
#endif        
        return Buffer;
}

static char *
DimNamesText
(
	NgShapeInfoGridRec *sirp
)
{
	int	i;
        NclApiVarInfoRec *vinfo = sirp->vinfo;
        char *cp;
        
        Buffer[0] = '\0';
	for (i=0; i < vinfo->n_dims; i++) {
                int clen;
                if (vinfo->dim_info[i].dim_quark <= NrmNULLQUARK)
                        cp = Unnamed;
                else
                        cp = NrmQuarkToString(vinfo->dim_info[i].dim_quark);
                clen = strlen(cp);
                while (clen + strlen(Buffer) + 2> Buflen) {
                        Buffer = NhlRealloc(Buffer,Buflen+BUFINC);
                }
		strcat(Buffer,cp);
                strcat(Buffer,"|");
                sirp->cwidths[i+1] = MAX(sirp->cwidths[i+1],clen);
	}
        Buffer[strlen(Buffer)-1] = '\0';
#if DEBUG_SHAPE_INFO_GRID      
        fprintf(stderr,"%s\n",Buffer);
#endif        
        return Buffer;
}

static char *
SelectedText
(
	NgShapeInfoGridRec *sirp
)
{
	int	i;
	char	dimstr[128];
        NgShapeInfoGrid *sip = &sirp->shapeinfogrid;
        NclApiVarInfoRec *vinfo = sirp->vinfo;
#if 0
	strcpy(Buffer,"Selected");
        sirp->cwidths[0] = MAX(sirp->cwidths[0],strlen(Buffer));
#endif
        sirp->shape = 0;
        sirp->cur_size = 1;
        sirp->total_size = 1;
        Buffer[0] = '\0';
	for (i=0; i < vinfo->n_dims; i++) {
                int size;
                size = 1 +abs((sip->finish[i] - sip->start[i])
                              / sip->stride[i]);
                if (size > 1)
                        sirp->shape++;
                sirp->cur_size *= size;
                sirp->total_size *= vinfo->dim_info[i].dim_size;
		sprintf(dimstr,"%d of %d",size,vinfo->dim_info[i].dim_size);
		strcat(Buffer,dimstr);
                strcat(Buffer,"|");
                sirp->cwidths[i+1] = MAX(sirp->cwidths[i+1],strlen(dimstr));
	}
        Buffer[strlen(Buffer)-1] = '\0';
#if DEBUG_SHAPE_INFO_GRID      
        fprintf(stderr,"%s\n",Buffer);
#endif
        
        return Buffer;
}
 
static char *
SubscriptText
(
	NgShapeInfoGridRec *sirp
)
{
	int	i;
	char	dimstr[128];
        NgShapeInfoGrid *sip = &sirp->shapeinfogrid;
        NclApiVarInfoRec *vinfo = sirp->vinfo;

	strcpy(Buffer,"Start/End/Stride");
        sirp->cwidths[0] = MAX(sirp->cwidths[0],strlen(Buffer));
        
        Buffer[0] = '\0';
	for (i=0; i < vinfo->n_dims; i++) {
                strcat(Buffer,"|");
		sprintf(dimstr,"%ld:%ld:%ld",
                        sip->start[i],sip->finish[i],sip->stride[i]);
		strcat(Buffer,dimstr);
                sirp->cwidths[i+1] = MAX(sirp->cwidths[i+1],strlen(dimstr));
	}
#if DEBUG_SHAPE_INFO_GRID      
        fprintf(stderr,"%s\n",Buffer);
#endif        
        return Buffer;
}
 
static char *
StartText
(
	NgShapeInfoGridRec *sirp
)
{
	int	i;
	char	dimstr[128];
        NgShapeInfoGrid *sip = &sirp->shapeinfogrid;
        NclApiVarInfoRec *vinfo = sirp->vinfo;

#if 0
	strcpy(Buffer,"Start");
        sirp->cwidths[0] = MAX(sirp->cwidths[0],strlen(Buffer));
#endif        
        Buffer[0] = '\0';
	for (i=0; i < vinfo->n_dims; i++) {
                if (sip->index_mode || vinfo->coordnames[i] <= NrmNULLQUARK) {
                        sprintf(dimstr,"%ld",sip->start[i]);
                }
                else if (sirp->float_types[i]) {
		  	sprintf(dimstr,"%.*g",FLT_DIG,sirp->start_coords[i]);
                        NgFixFloat(dimstr);
                        NgRemoveZeros(dimstr);
                }
                else {
                        sprintf(dimstr,"%ld",(long)sirp->start_coords[i]);
                }
		strcat(Buffer,dimstr);
                strcat(Buffer,"|");
                sirp->cwidths[i+1] = MAX(sirp->cwidths[i+1],strlen(dimstr));
	}
        Buffer[strlen(Buffer)-1] = '\0';
#if DEBUG_SHAPE_INFO_GRID      
        fprintf(stderr,"%s\n",Buffer);
#endif        
        return Buffer;
}


static char *
FinishText
(
	NgShapeInfoGridRec *sirp
)
{
	int	i;
	char	dimstr[128];
        NgShapeInfoGrid *sip = &sirp->shapeinfogrid;
        NclApiVarInfoRec *vinfo = sirp->vinfo;
#if 0
	strcpy(Buffer,"Finish");
        sirp->cwidths[0] = MAX(sirp->cwidths[0],strlen(Buffer));
#endif        
        Buffer[0] = '\0';
        for (i=0; i < vinfo->n_dims; i++) {
                if (sip->index_mode || vinfo->coordnames[i] <= NrmNULLQUARK) {
                        sprintf(dimstr,"%ld",sip->finish[i]);
                }
                else if (sirp->float_types[i]) {
		  	sprintf(dimstr,"%.*g",FLT_DIG,sirp->finish_coords[i]);
                        NgFixFloat(dimstr);
                        NgRemoveZeros(dimstr);
                }
                else {
                        sprintf(dimstr,"%ld",(long)sirp->finish_coords[i]);
                }
                strcat(Buffer,dimstr);
                strcat(Buffer,"|");
                sirp->cwidths[i+1] = MAX(sirp->cwidths[i+1],strlen(dimstr));
	}
        Buffer[strlen(Buffer)-1] = '\0';
#if DEBUG_SHAPE_INFO_GRID      
        fprintf(stderr,"%s\n",Buffer);
#endif        
        return Buffer;
}

static char *
StrideText
(
	NgShapeInfoGridRec *sirp
)
{
	int	i;
	char	dimstr[128];
        NgShapeInfoGrid *sip = &sirp->shapeinfogrid;
        NclApiVarInfoRec *vinfo = sirp->vinfo;
#if 0
	strcpy(Buffer,"Stride");
        sirp->cwidths[0] = MAX(sirp->cwidths[0],strlen(Buffer));
#endif        
        Buffer[0] = '\0';
	for (i=0; i < vinfo->n_dims; i++) {
		sprintf(dimstr,"%ld",sip->stride[i]);
		strcat(Buffer,dimstr);
                strcat(Buffer,"|");
                sirp->cwidths[i+1] = MAX(sirp->cwidths[i+1],strlen(dimstr));
	}
        Buffer[strlen(Buffer)-1] = '\0';
#if DEBUG_SHAPE_INFO_GRID      
        fprintf(stderr,"%s\n",Buffer);
#endif        
        return Buffer;
}



static	NclExtValueRec *
ReadCoord
(
        NclApiVarInfoRec	*vinfo,
        NrmQuark		qfile,
	int			dim_ix,
	long			*start,
	long			*finish,
	long			*stride
)
{
	NclExtValueRec *val;

        if (vinfo->coordnames[dim_ix] <= NrmNULLQUARK)
                return NULL;
        
        if (qfile) {
                val = NclReadFileVarCoord(qfile,vinfo->name,
                                          vinfo->coordnames[dim_ix],
                                          start,finish,stride);
        }
        else {
                val = NclReadVarCoord(vinfo->name,
                                      vinfo->coordnames[dim_ix],
                                      start,finish,stride);
        }
        return val;

}

static double
ValToDouble
(
        NclExtValueRec	*val,
        int		index
        )
{
        char *valp = ((char *) val->value) + index * val->elem_size;
        double dout;

        switch (val->type) {
            case NCLAPI_float:
                    dout = (double)*(float*)valp;
                    return dout;
            case NCLAPI_double:
                    dout = *(double*)valp;
                    return dout;
            case NCLAPI_byte:
            case NCLAPI_char:
                    dout = (double)*(char*)valp;
                    return dout;
            case NCLAPI_int:
                    dout = (double)*(int*)valp;
                    return dout;
            case NCLAPI_short:
                    dout = (double)*(short*)valp;
                    return dout;
            case NCLAPI_long:
                    dout = (double)*(long*)valp;
                    return dout;
            default:
                    printf("type not supported for coordinate variable\n");
        }
        return 0.0;
}

static void
SetSelectedDim
(
        NgShapeInfoGrid *sip,
        int selected_dim
        )
{
        NgShapeInfoGridRec *sirp = (NgShapeInfoGridRec *) sip;
        static NhlBoolean first = True;
        
        if (first) {
                XmLGridColumn colptr;
                XmLGridRow rowptr;

                first = False;
                
        
                colptr = XmLGridGetColumn(sip->grid,XmCONTENT,0);
                rowptr = XmLGridGetRow(sip->grid,XmHEADING,0);
                XtVaGetValues(sip->grid,
                              XmNcolumnPtr,colptr,
                              XmNrowPtr,rowptr,
                              XmNcellForeground,&Foreground,
                              XmNcellBackground,&Background,
                              XmNcellRightBorderColor,&Right_Border_Color,
                              XmNcellLeftBorderColor,&Left_Border_Color,
                              NULL);
        }

                    /* restore last selected */
        XtVaSetValues(sip->grid,
                      XmNcolumn,sip->selected_dim,
                      XmNrow,DIM_NAMES_ROW,
                      XmNrowType,XmHEADING,
                      XmNcellForeground,Foreground,
                      XmNcellBackground,Background,
                      NULL);
        XtVaSetValues(sip->grid,
                      XmNcolumn,sip->selected_dim,
                      XmNrowRangeStart,START_ROW,
                      XmNrowRangeEnd,STRIDE_ROW,
		      XmNcellBackground,sirp->go->go.edit_field_pixel,
                      XmNcellRightBorderColor,Right_Border_Color,
                      XmNcellLeftBorderColor,Left_Border_Color,
                      NULL);

            /* set new selected */
        XtVaSetValues(sip->grid,
                      XmNcolumn,selected_dim,
                      XmNrow,DIM_NAMES_ROW,
                      XmNrowType,XmHEADING,
                      XmNcellForeground,Background,
                      XmNcellBackground,Foreground,
                      NULL);
        XtVaSetValues(sip->grid,
                      XmNcolumn,selected_dim,
                      XmNrowRangeStart,START_ROW,
                      XmNrowRangeEnd,STRIDE_ROW,
		      XmNcellRightBorderColor,sirp->go->go.select_pixel,
		      XmNcellLeftBorderColor,sirp->go->go.select_pixel,
                      NULL);

        if (selected_dim != sip->selected_dim) {
                sip->synchro_step = False;
                sip->selected_dim = selected_dim;
        }
        

        if (sip->edit_row > -1) {
                if (sip->synchro_step && sip->edit_row != STRIDE_ROW)
                        XtVaSetValues(sip->grid,
                                      XmNrowRangeStart,START_ROW,
                                      XmNrowRangeEnd,FINISH_ROW,
                                      XmNcolumn,sip->selected_dim,
				      XmNcellBackground,
				      sirp->go->go.select_pixel,
                                      NULL);
                else
                        XtVaSetValues(sip->grid,
                                      XmNrow,sip->edit_row,
                                      XmNcolumn,sip->selected_dim,
				      XmNcellBackground,
				      sirp->go->go.select_pixel,
                                      NULL);
                XmLGridSetFocus(sip->grid,sip->edit_row,sip->selected_dim);
                
        }
        
        return;

}

static void
SelectDimCB
(
	Widget		w,
	XtPointer	data,
	XtPointer	cb_data
)
{
        NgShapeInfoGrid *sip = (NgShapeInfoGrid *) data;
        XmLGridCallbackStruct *cb = (XmLGridCallbackStruct *) cb_data;
        
        if (cb->column == sip->selected_dim ||
            cb->columnType != XmCONTENT)
                return;
        
        SetSelectedDim(sip,cb->column);
        
        (*sip->dim_select_notify)(sip->notify_data);
        
        return;
}

static void
CellFocusCB
(
	Widget		w,
	XtPointer	data,
	XtPointer	cb_data
)
{
        NgShapeInfoGridRec *sirp = (NgShapeInfoGridRec *) data;
        NgShapeInfoGrid *sip = (NgShapeInfoGrid *) data;
        XmLGridCallbackStruct *cb = (XmLGridCallbackStruct *) cb_data;
        int start_row,finish_row;

	if (sirp->ignore_focus_cb) 
		return;

        if (cb->reason == XmCR_CELL_FOCUS_OUT) {
                XtVaSetValues(sip->grid,
                              XmNrowRangeStart,START_ROW,
                              XmNrowRangeEnd,STRIDE_ROW,
                              XmNcolumn,cb->column,
			      XmNcellBackground,sirp->go->go.edit_field_pixel,
                              NULL);
        }
        else {
                if (sip->start[cb->column] != sip->finish[cb->column] ||
		    sip->selected_dim != cb->column)
                        sip->synchro_step = False;
                if (sip->synchro_step && cb->row != STRIDE_ROW) {
                        start_row = START_ROW;
                        finish_row = FINISH_ROW;
                }
                else {
                        start_row = cb->row;
                        finish_row = cb->row;
                }
                sip->edit_row = cb->row;
                XtVaSetValues(sip->grid,
                              XmNrowRangeStart,start_row,
                              XmNrowRangeEnd,finish_row,
                              XmNcolumn,cb->column,
			      XmNcellBackground,sirp->go->go.select_pixel,
                              NULL);
		if (sip->selected_dim != cb->column)
			SetSelectedDim(sip,cb->column);
		(*sip->dim_select_notify)(sip->notify_data);
        }
        return;
}

static void
UpdateCoordInfo
(
        NgShapeInfoGrid *sip
        )
{
        NgShapeInfoGridRec *sirp = (NgShapeInfoGridRec *) sip;
        int i;

        if (sirp->coords_alloced < sirp->vinfo->n_dims) {
                sirp->start_coords = NhlRealloc
                        (sirp->start_coords,
                         sirp->vinfo->n_dims * sizeof(float));
                sirp->finish_coords = NhlRealloc
                        (sirp->finish_coords,
                         sirp->vinfo->n_dims * sizeof(float));
                sirp->float_types = NhlRealloc
                        (sirp->float_types,
                         sirp->vinfo->n_dims * sizeof(Boolean));
                sirp->coords_alloced = sirp->vinfo->n_dims;
        }
        for (i = 0; i < sirp->vinfo->n_dims; i++) {
                NclExtValueRec *val;
                
                if (sirp->vinfo->coordnames[i] <= NrmNULLQUARK)
                        continue;
                val = ReadCoord(sirp->vinfo,sirp->qfileref,i,
                                &sip->start[i],&sip->finish[i],
                                &sip->stride[i]);
                if (val) {
                        sirp->start_coords[i] = ValToDouble(val,0);
                        sirp->finish_coords[i] =
                                ValToDouble(val,val->totalelements-1);
                        sirp->float_types[i] =
                                (val->type == NCLAPI_float ||
                                 val->type == NCLAPI_double) ?
                                True : False;
                        if (val->constant != 0)
                                NclFree(val->value);
                        NclFreeExtValue(val);
                }
        }
        
        return;
}


static NhlBoolean
UpdateCoordValue
(
        NgShapeInfoGrid *sip,
        char *coordstring,
        int row,
        int column
        )
{
        NgShapeInfoGridRec *sirp = (NgShapeInfoGridRec *) sip;
        NclExtValueRec *val;
        double coordval;
	NhlBoolean ascending_coords,ascending_bounds;
	double firstval,lastval;
	double fmin, fmax;
	double fstart,ffinish;
        int i,imin,imax;
	int dimsize = sirp->vinfo->dim_info[column].dim_size;
        char *sval;
        int slen;
        NhlBoolean retval;

        
	val = ReadCoord(sirp->vinfo,sirp->qfileref,column,NULL,NULL,NULL);
        if (! val)
                return False;
        
        sscanf(coordstring,"%lf",&coordval);
        switch (row) {
            case START_ROW:
                    fstart = coordval;
                    ffinish = sip->synchro_step ?
			    coordval : ValToDouble(val,sip->finish[column]);
                    break;
            case FINISH_ROW:
                    fstart = sip->synchro_step ?
			    coordval : ValToDouble(val,sip->start[column]);
                    ffinish = coordval;
                    break;
        }

	firstval = (double) ValToDouble(val,0);
	lastval = (double) ValToDouble(val,dimsize-1);

	ascending_coords = lastval > firstval;
	ascending_bounds = ffinish > fstart;

	fmin = ascending_bounds ? fstart : ffinish;
	fmin = ascending_coords ? MAX(fmin,firstval) : MAX(fmin,lastval);
	fmax = ascending_bounds ? ffinish : fstart;
	fmax = ascending_coords ? MIN(fmax,lastval) : MIN(fmax,firstval);

	imin = 0;
	imax = dimsize - 1;
	if (ascending_coords) {
		for (i = 0;i<dimsize;i++) {
			double coord = ValToDouble(val,i);
			if (fmin <= coord) {
				imin = i;
				break;
			}
		}
		for (i = dimsize-1;i>-1;i--) {
			double coord = ValToDouble(val,i);
			if (fmax >= coord) {
				imax = MAX(i,imin);
				break;
			}
		}
	}
	else {
		for (i = dimsize-1;i>-1;i--) {
			double coord = ValToDouble(val,i);
			if (fmin <= coord) {
				imin = i;
				break;
			}
		}
		for (i = 0;i<dimsize;i++) {
			double coord = ValToDouble(val,i);
			if (fmax >= coord) {
				imax = MAX(imin,i);
				break;
			}
		}
	}

        if (ascending_bounds) {
                if (imin == sip->start[column] &&
                    imax == sip->finish[column])
                        retval = False;
                else {
                        sip->start[column] = imin;
                        sip->finish[column] = imax;
                        retval = True;
                }
        }
        else {
                if (imax == sip->start[column] &&
                    imin == sip->finish[column])
                        retval = False;
                else {
                        sip->start[column] = imax;
                        sip->finish[column] = imin;
                        retval = True;
                }
        }
        if (retval) {
                    /* make sure float values are in canonical form */
                switch (row) {
                    case START_ROW:
                            sval = NgTypedValueToString
                                    (val,sip->start[column],True,&slen);
                            XmLGridSetStringsPos
                                    (sip->grid,XmCONTENT,START_ROW,
                                     XmCONTENT,column,sval);
			    if (sip->synchro_step)
				    XmLGridSetStringsPos
					    (sip->grid,XmCONTENT,FINISH_ROW,
					     XmCONTENT,column,sval);
                            break;
                    case FINISH_ROW:
                            sval = NgTypedValueToString
                                    (val,sip->finish[column],True,&slen);
                            XmLGridSetStringsPos
                                    (sip->grid,XmCONTENT,FINISH_ROW,
                                     XmCONTENT,column,sval);
			    if (sip->synchro_step)
				    XmLGridSetStringsPos
					    (sip->grid,XmCONTENT,START_ROW,
					     XmCONTENT,column,sval);
                            break;
                }
        }
	if (val->constant != 0)
		NclFree(val->value);
	NclFreeExtValue(val);
        
        return retval;
}

static NhlBoolean
UpdateIndexValue
(
        NgShapeInfoGrid *sip,
        char *indexstring,
        int row,
        int column
        )
{
        NgShapeInfoGridRec *sirp = (NgShapeInfoGridRec *) sip;
	int add_row = -1;
        int index;

        sscanf(indexstring,"%d",&index);
        switch (row) {
            case START_ROW:
                    if (index == sip->start[column] || index < 0 ||
                        index >= sirp->vinfo->dim_info[column].dim_size) {
                            return False;
                    }
                    sip->start[column] = index;
		    if (sip->synchro_step) {
			    sip->finish[column] = index;
			    add_row = FINISH_ROW;
		    }
                    break;
            case FINISH_ROW:
                    if (index == sip->finish[column] ||
                        index < 0 ||
                        index >= sirp->vinfo->dim_info[column].dim_size) {
                            return False;
                    }
                    sip->finish[column] = index;
		    if (sip->synchro_step) {
			    sip->start[column] = index;
			    add_row = START_ROW;
		    }
                    break;
            case STRIDE_ROW:
                    if (index == sip->stride[column] ||
                        index < 0 ||
                        index > sirp->vinfo->dim_info[column].dim_size) {
                            return False;
                    }
                    sip->stride[column] = index;
                    break;
        }
        sprintf(Buffer,"%d",index);
        XmLGridSetStringsPos(sip->grid,XmCONTENT,row,
                             XmCONTENT,column,Buffer);
	if (add_row > -1) {
		XmLGridSetStringsPos(sip->grid,XmCONTENT,add_row,
				     XmCONTENT,column,Buffer);
	}
        return True;
}

static void
UpdateState
(
        NgShapeInfoGrid *sip,
        int		row,
        int		col,
	NhlBoolean	notify_req
        )
{
        NgShapeInfoGridRec *sirp = (NgShapeInfoGridRec *) sip;
        int start_row,finish_row;
        
        XmLGridSetStringsPos(sip->grid,XmHEADING,SELECTED_ROW,XmCONTENT,0,
                             SelectedText(sirp));
        XmLGridSetStringsPos(sip->grid,XmHEADING,SELECTED_ROW,XmFOOTER,0,
                             TotalText(sirp));
        XmLGridSetStringsPos
                (sip->grid,XmCONTENT,STRIDE_ROW,XmFOOTER,0,ShapeText(sirp));
        
        XtVaSetValues(sip->grid,
                      XmNsimpleWidths,ColumnWidths(sirp),
                      NULL);
        
        if (sip->synchro_step &&
            (row == START_ROW ||row == FINISH_ROW)) {
                start_row = START_ROW;
                finish_row = FINISH_ROW;
        }
        else {
                start_row = row;
                finish_row = row;
                XtVaSetValues(sip->grid,
                              XmNrowRangeStart,start_row,
                              XmNrowRangeEnd,finish_row,
                              XmNcolumn,col,
			      XmNcellBackground,sirp->go->go.edit_field_pixel,
                              NULL);
		if (! sip->synchro_step)
			sip->edit_row = -1;
        }

        if (notify_req)
		(*sip->shape_notify)(sip->notify_data);
        return;
        
}

static void
DimEditCB
(
	Widget		w,
	XtPointer	data,
	XtPointer	cb_data
)
{
        NgShapeInfoGridRec *sirp = (NgShapeInfoGridRec *) data;
        NgShapeInfoGrid *sip = (NgShapeInfoGrid *) data;
        XmLGridCallbackStruct *cb = (XmLGridCallbackStruct *) cb_data;
        Widget tw;
        char *string;
        static XmString cell_string = NULL;
        NhlBoolean update = True;
        XmLGridColumn colptr;
        XmLGridRow rowptr;
        Widget text;
#if DEBUG_SHAPE_INFO_GRID      
	NhlBoolean insert_mode = False;
#endif
        
        switch (cb->reason) {
            case XmCR_EDIT_INSERT:
#if DEBUG_SHAPE_INFO_GRID      
		    insert_mode = True;
#endif
            case XmCR_EDIT_BEGIN:
#if DEBUG_SHAPE_INFO_GRID      
                    fprintf(stderr,"edit begin %s\n",
			    insert_mode ? "insert mode" : "");
#endif
                    colptr = XmLGridGetColumn(sip->grid,XmCONTENT,cb->column);
                    rowptr = XmLGridGetRow(sip->grid,XmCONTENT,cb->row);
                    if (cell_string)
                            XmStringFree(cell_string);
                    XtVaGetValues
                            (sip->grid,
                             XmNcolumnPtr,colptr,
                             XmNrowPtr,rowptr,
                             XmNcellString,&cell_string,
                             NULL);
        
                    XtVaGetValues(sip->grid,
                                  XmNtextWidget,&text,
                                  NULL);
                    XtVaSetValues(text,
				  XmNbackground,sirp->go->go.select_pixel,
                                  NULL);
                    sip->edit_row = cb->row;
		    sirp->manual_edit_started = True;
                    return;
            case XmCR_EDIT_CANCEL:
#if DEBUG_SHAPE_INFO_GRID      
                    fprintf(stderr,"edit cancel\n");
#endif
                    update = False;
		    sirp->manual_edit_started = False;
                    break;
            case XmCR_EDIT_COMPLETE:
#if DEBUG_SHAPE_INFO_GRID      
                    fprintf(stderr,"edit complete\n");
#endif
		    sirp->manual_edit_started = False;
                    break;
        }

        if (update) {
                XtVaGetValues(w,
                              XmNtextWidget,&tw,
                              NULL);
                string = XmTextGetString(tw);
                
                if (sip->index_mode ||
                    sirp->vinfo->coordnames[cb->column] <= NrmNULLQUARK ||
                    cb->row == STRIDE_ROW) {
                        update = UpdateIndexValue
                                (sip,string,cb->row,cb->column);
                }
                else {
                        update = UpdateCoordValue
                                (sip,string,cb->row,cb->column);
                }
        }
        if (!update) {
                XtVaSetValues(sip->grid,
                              XmNrow,cb->row,
                              XmNcolumn,cb->column,
                              XmNcellString,cell_string,
			      XmNcellBackground,sirp->go->go.edit_field_pixel,
                              NULL);
                return;
        }
        else if (cb->reason == XmCR_EDIT_COMPLETE) {
		UpdateState(sip,cb->row,cb->column,True);
        }
	sip->edit_row = -1;
        return;
}

static int
NewIndexValue
(
        NgShapeInfoGrid *sip,
        unsigned char	how,
        int		row,
        int		column
        )
{
        NgShapeInfoGridRec *sirp = (NgShapeInfoGridRec *) sip;
        int index;
        int dim_size = sirp->vinfo->dim_info[column].dim_size;

        switch (how) {
            case NG_INCREMENT:
                    switch (row) {
                        case START_ROW:
                                index = MIN(sip->start[column]+1,dim_size-1);
                                sip->start[column] = index;
                                break;
                        case FINISH_ROW:
                                index = MIN(sip->finish[column]+1,dim_size-1);
                                sip->finish[column] = index;
                                break;
                        case STRIDE_ROW:
                                index = MIN(sip->stride[column]+1,dim_size);
                                sip->stride[column] = index;
                                break;
                    }
                    break;
            case NG_DECREMENT:
                    switch (row) {
                        case START_ROW:
                                index = MAX(sip->start[column]-1,0);
                                sip->start[column] = index;
                                break;
                        case FINISH_ROW:
                                index = MAX(sip->finish[column]-1,0);
                                sip->finish[column] = index;
                                break;
                        case STRIDE_ROW:
                                index = MAX(sip->stride[column]-1,1);
                                sip->stride[column] = index;
                                break;
                    }
                    break;
            case NG_MAX_VAL:
                    switch (row) {
                        case START_ROW:
                                index = dim_size-1;
                                sip->start[column] = index;
                                break;
                        case FINISH_ROW:
                                index = dim_size-1;
                                sip->finish[column] = index;
                                break;
                        case STRIDE_ROW:
                                index = dim_size;
                                sip->stride[column] = index;
                                break;
                    }
                    break;
            case NG_MIN_VAL:
                    switch (row) {
                        case START_ROW:
                                index = 0;
                                sip->start[column] = index;
                                break;
                        case FINISH_ROW:
                                index = 0;
                                sip->finish[column] = index;
                                break;
                        case STRIDE_ROW:
                                index = 1;
                                sip->stride[column] = index;
                                break;
                    }
                    break;
            case NG_MATCH_VAL:
                    switch (row) {
                        case START_ROW:
                                index = sip->finish[column];
                                sip->start[column] = index;
                                break;
                        case FINISH_ROW:
                                index = sip->start[column];
                                sip->finish[column] = index;
                                break;
                        case STRIDE_ROW:
                                index = sip->stride[column];
                        default:
                                break;
                    }
                    break;
            case NG_STRIDE_INC:
                    switch (row) {
                        case START_ROW:
                                index = MIN(sip->start[column]
                                            +sip->stride[column],dim_size-1);
                                sip->start[column] = index;
                                break;
                        case FINISH_ROW:
                                index = MIN(sip->finish[column]
                                            +sip->stride[column],dim_size-1);
                                sip->finish[column] = index;
                                break;
                        case STRIDE_ROW:
                                index = sip->stride[column];
                                break;
                    }
                    break;
            case NG_STRIDE_DEC:
                    switch (row) {
                        case START_ROW:
                                index = MAX(sip->start[column]
                                            -sip->stride[column],0);
                                sip->start[column] = index;
                                break;
                        case FINISH_ROW:
                                index = MAX(sip->finish[column]
                                            -sip->stride[column],0);
                                sip->finish[column] = index;
                                break;
                        case STRIDE_ROW:
                                index = sip->stride[column];
                                break;
                    }
                    break;
        }
        return index;
}

static NhlString
NewCoordValue
(
        NgShapeInfoGrid *sip,
        unsigned char	how,
        int		row,
        int		column
        )
{
        NgShapeInfoGridRec *sirp = (NgShapeInfoGridRec *) sip;
        int index;
        char *sval;
        int slen;
        static NrmQuark qfile = NrmNULLQUARK, qvar = NrmNULLQUARK;
        static int last_column = -1;
        static NclExtValueRec *val = NULL;

        if (! (val && qfile == sirp->qfileref && 
	       sirp->vinfo->name == qvar && column == last_column)) {
                last_column = column;
		qfile = sirp->qfileref;
		qvar = sirp->vinfo->name;

                if (val) {
                        if (val->constant != 0)
                                NclFree(val->value);
                        NclFreeExtValue(val);
                }
                val = ReadCoord(sirp->vinfo,
                                sirp->qfileref,column,NULL,NULL,NULL);
        }
	if (! val) {
		NHLPERROR((NhlFATAL,ENOMEM,NULL));
		return NULL;
	}
        index = NewIndexValue(sip,how,row,column);
        sval = NgTypedValueToString(val,index,True,&slen);

        return sval;
}

NhlErrorTypes NgShapeInfoGridEditFocusCell
(
        NgShapeInfoGrid		*shape_info_grid,
        unsigned char		how,
        Boolean			synchro_mode_update
        )
{
        NgShapeInfoGridRec *sirp;
        NgShapeInfoGrid *sip;
        int row,col,start_row,finish_row;
        
        sirp = (NgShapeInfoGridRec *) shape_info_grid;
        if (!sirp) return NhlFATAL;
        sip = &sirp->shapeinfogrid;

        if (sip->edit_row == -1)
                return NhlNOERROR;

        row = sip->edit_row;
        col = sip->selected_dim;
        if (how > NG_MIN_VAL)
                return NhlFATAL;
        
        if (sip->synchro_step &&
            (row == START_ROW || row == FINISH_ROW)) {
                start_row = START_ROW;
                finish_row = FINISH_ROW;
        }
        else {
                start_row = row;
                finish_row = row;
        }
        
        XtVaSetValues(sip->grid,
                      XmNrowRangeStart,start_row,
                      XmNrowRangeEnd,finish_row,
                      XmNcolumn,col,
		      XmNcellBackground,sirp->go->go.select_pixel,
                      NULL);
/*
 * this is kind of tricky. If in synchro mode then UpdateState is called
 * to propagate the date change to target data receptors (e.g. plots or the
 * datagrid). This actually happens on the arm callback of the Vcr buttons,
 * which calls the function with synchro_mode_update set True. However,
 * when editing the stride field the state should not be updated immediately.
 * So in this case the syncho_mode_update becomes a noop.
 */

        if (synchro_mode_update && 
	    how != NG_MIN_VAL && 
	    how != NG_MAX_VAL) {
		if (row != STRIDE_ROW) 
			UpdateState(shape_info_grid,row,col,True);
                return NhlNOERROR;
        }
        if (! sip->synchro_step || row == STRIDE_ROW) {
                if (sip->index_mode ||
                    sirp->vinfo->coordnames[col] <= NrmNULLQUARK ||
                    row == STRIDE_ROW) {
                        sprintf(Buffer,"%d",NewIndexValue(sip,how,row,col));
                        XmLGridSetStringsPos(sip->grid,XmCONTENT,row,
                                             XmCONTENT,col,Buffer);
                }
                else {
                        sprintf(Buffer,"%s",NewCoordValue(sip,how,row,col));
                        XmLGridSetStringsPos(sip->grid,XmCONTENT,row,
                                             XmCONTENT,col,Buffer);
                }
                return NhlNOERROR;
        }
            /* else if synchro_step */
        
        if (how == NG_INCREMENT)
                how = NG_STRIDE_INC;
        else if (how == NG_DECREMENT)
                how = NG_STRIDE_DEC;
        
        if (sip->index_mode ||
            sirp->vinfo->coordnames[col] <= NrmNULLQUARK) {
                sprintf(Buffer,"%d",NewIndexValue(sip,how,START_ROW,col));
                XmLGridSetStringsPos(sip->grid,XmCONTENT,START_ROW,
                                     XmCONTENT,col,Buffer);
                sprintf(Buffer,"%d",NewIndexValue(sip,how,FINISH_ROW,col));
                XmLGridSetStringsPos(sip->grid,XmCONTENT,FINISH_ROW,
                                     XmCONTENT,col,Buffer);
        }
        else {
                sprintf(Buffer,"%s",NewCoordValue(sip,how,START_ROW,col));
                XmLGridSetStringsPos(sip->grid,XmCONTENT,START_ROW,
                                     XmCONTENT,col,Buffer);
                sprintf(Buffer,"%s",NewCoordValue(sip,how,FINISH_ROW,col));
                XmLGridSetStringsPos(sip->grid,XmCONTENT,FINISH_ROW,
                                     XmCONTENT,col,Buffer);
        }

        if (how == NG_MIN_VAL || how == NG_MAX_VAL) {
                UpdateState(shape_info_grid,row,col,True);
        }
        return NhlNOERROR;
}

NhlErrorTypes NgShapeInfoGridEditFocusCellComplete
(
        NgShapeInfoGrid		*shape_info_grid,
	NhlBoolean		notify
        )
{
        NgShapeInfoGridRec *sirp;
        NgShapeInfoGrid *sip;
        
        sirp = (NgShapeInfoGridRec *) shape_info_grid;
        if (!sirp) return NhlFATAL;
        sip = &sirp->shapeinfogrid;

        if (sip->edit_row < 0)
		return NhlFATAL;

	if (sirp->manual_edit_started)
		XmLGridEditComplete(sip->grid);

        UpdateState(shape_info_grid,sip->edit_row,sip->selected_dim,notify);
        if (! sip->synchro_step)
                sip->edit_row = -1;
	else if (sip->edit_row == STRIDE_ROW) {
		sip->edit_row = START_ROW;
                XtVaSetValues(sip->grid,
                              XmNrowRangeStart,START_ROW,
                              XmNrowRangeEnd,FINISH_ROW,
                              XmNcolumn,sip->selected_dim,
			      XmNcellBackground,sirp->go->go.select_pixel,
                              NULL);
	}
        
        return NhlNOERROR;
}

NhlErrorTypes NgShapeInfoGridSynchroStepMode
(
        NgShapeInfoGrid		*shape_info_grid,
        NhlBoolean		on
        )
{
        NgShapeInfoGridRec *sirp;
        NgShapeInfoGrid *sip;
        int row,col;
        Boolean focus;
        
#if DEBUG_SHAPE_INFO_GRID      
        fprintf(stderr,"synchro step mode %s\n",on ? "True" : "False");
#endif
        
        sirp = (NgShapeInfoGridRec *) shape_info_grid;
        if (!sirp) return NhlFATAL;
        sip = &sirp->shapeinfogrid;

        sip->synchro_step = on;
         
	if (sip->edit_row < 0) {
		XmLGridGetFocus(sip->grid,&row,&col,&focus);
        	if (row < 0 || col < 0)
                	return NhlNOERROR;
	}
	else {
		row = sip->edit_row;
		col = sip->selected_dim;
	}
        
        XtVaSetValues(sip->grid,
                      XmNrowRangeStart,START_ROW,
                      XmNrowRangeEnd,FINISH_ROW,
                      XmNcolumn,col,
		      XmNcellBackground,sirp->go->go.edit_field_pixel,
                      NULL);
	if (! on) {
		if (sip->edit_row > -1) {
			XtVaSetValues(sip->grid,
				      XmNrow,sip->edit_row,
				      XmNcolumn,sip->selected_dim,
				      XmNcellBackground,
				      sirp->go->go.select_pixel,
				      NULL);
		}
		return NhlNOERROR;
	}
        
	sip->edit_row = row;
        if (sip->synchro_step && sip->edit_row != STRIDE_ROW) {
		UpdateState(shape_info_grid,
			    sip->edit_row,sip->selected_dim,False);
		NgShapeInfoGridEditFocusCellComplete(shape_info_grid,False);
                XtVaSetValues(sip->grid,
                              XmNrowRangeStart,START_ROW,
                              XmNrowRangeEnd,FINISH_ROW,
                              XmNcolumn,sip->selected_dim,
			      XmNcellBackground,sirp->go->go.select_pixel,
                              NULL);
        }
        return NhlNOERROR;
}

void
NgSetShapeInfoGridSetFocusCell
(
        NgShapeInfoGrid		*shape_info_grid
        )

{
        if (shape_info_grid->edit_row > -1)
                XmLGridSetFocus(shape_info_grid->grid,
                                shape_info_grid->edit_row,
                                shape_info_grid->selected_dim);

        return;
}


NhlErrorTypes NgUpdateShapeInfoGrid
(
        NgShapeInfoGrid		*shape_info_grid,
        NrmQuark		qfileref,
	NrmQuark		qvar
        )
{
        NgShapeInfoGridRec *sirp;
        NgShapeInfoGrid *sip;
        int	i;
        static NhlBoolean first = True;
	NhlBoolean newvar = False;
        
        sirp = (NgShapeInfoGridRec *) shape_info_grid;
        if (!sirp) return NhlFATAL;
        sip = &sirp->shapeinfogrid;

        if (first) {
                first = False;
                Buffer = NhlMalloc(BUFINC);
                Buflen = BUFINC;
        }


        if (sirp->qfileref != qfileref || sirp->qvar != qvar) {
                sip->edit_row = -1;
		sirp->manual_edit_started = False;
		newvar = True;
	}
        sirp->qfileref = qfileref;
	sirp->qvar = qvar;
	if (newvar || ! sirp->dl) {
		if (sirp->dl)
			NclFreeDataList(sirp->dl);
		if (sirp->qfileref)
			sirp->dl = NclGetFileVarInfo
				(sirp->qfileref,sirp->qvar);
		else
			sirp->dl = NclGetVarInfo(sirp->qvar);
		if (sirp->dl)
			sirp->vinfo = sirp->dl->u.var;
	}
	if (! sirp->vinfo) {
		NHLPERROR((NhlFATAL,ENOMEM,NULL));
		return NhlFATAL;
	}
	
/*
 * a side-effect of this call is to set the focus, thus calling CellFocusCB,
 * and setting the focus cell; put this first to avoid changing the background
 * of the edit cell prematurely. Also save the selected dim so it does not
 * get incorrectly changed.
 */

	sirp->ignore_focus_cb = True;
        XtVaSetValues(sip->grid,
                      XmNcolumns,sirp->vinfo->n_dims,
                      NULL);
	sirp->ignore_focus_cb = False;
        
#if DEBUG_SHAPE_INFO_GRID
        fprintf(stderr,"shapeinfo rows %d vis rows %d height %d\n",
               nrows, nvisrows,height);
#endif

        UpdateCoordInfo(sip);
        XtVaSetValues(sip->grid,
                      XmNcolumnRangeStart,0,
                      XmNcolumnRangeEnd,sirp->vinfo->n_dims-1,
                      XmNrowRangeStart,START_ROW,
                      XmNrowRangeEnd,STRIDE_ROW,
                      XmNcellEditable,True,
		      XmNcellBackground,sirp->go->go.edit_field_pixel,
                      NULL);
        
        for (i = 0; i <= sirp->vinfo->n_dims; i++) {
                sirp->cwidths[i] = 0;
        }
        sirp->cwidths[0] = strlen("Dimensions");
        sirp->cwidths[sirp->vinfo->n_dims+1] = strlen("Total Elements");
        
        XmLGridSetStringsPos(sip->grid,XmHEADING,DIM_NAMES_ROW,XmCONTENT,0,
                             DimNamesText(sirp));
        XmLGridSetStringsPos(sip->grid,XmHEADING,SELECTED_ROW,XmCONTENT,0,
                             SelectedText(sirp));
        XmLGridSetStringsPos(sip->grid,XmCONTENT,START_ROW,XmCONTENT,0,
                             StartText(sirp));
        XmLGridSetStringsPos(sip->grid,XmCONTENT,FINISH_ROW,XmCONTENT,0,
                             FinishText(sirp));
        XmLGridSetStringsPos(sip->grid,XmCONTENT,STRIDE_ROW,XmCONTENT,0,
                             StrideText(sirp));
        XmLGridSetStringsPos(sip->grid,XmHEADING,SELECTED_ROW,XmFOOTER,0,
                             TotalText(sirp));
        XmLGridSetStringsPos
                (sip->grid,XmCONTENT,STRIDE_ROW,XmFOOTER,0,ShapeText(sirp));
        
        XtVaSetValues(sip->grid,
                      XmNsimpleWidths,ColumnWidths(sirp),
                      NULL);

        if (sip->selected_dim < 0 ||
            sip->selected_dim > sirp->vinfo->n_dims - 1)
                sip->selected_dim = sirp->vinfo->n_dims - 1;
        
        SetSelectedDim(sip,sip->selected_dim);
        
        return NhlNOERROR;
}

NgShapeInfoGrid *NgCreateShapeInfoGrid
(
        NgGO                    go,
        Widget			parent
        )
{
        NgShapeInfoGridRec *sirp;
        NgShapeInfoGrid *sip;
        int nrows;
        
        sirp = NhlMalloc(sizeof(NgShapeInfoGridRec));
        if (!sirp) return NULL;
        sip = &sirp->shapeinfogrid;

        nrows = 3;
        
        sip->grid = XtVaCreateManagedWidget
                ("ShapeInfoGrid",
                 xmlGridWidgetClass,parent,
                 XmNrows,nrows,
                 XmNselectionPolicy,XmSELECT_NONE,
                 XmNverticalSizePolicy,XmVARIABLE,
                 XmNhorizontalSizePolicy,XmVARIABLE,
                 NULL);
         
        XtAddCallback(sip->grid,XmNeditCallback,DimEditCB,sirp);
        XtAddCallback(sip->grid,XmNselectCallback,SelectDimCB,sirp);
        XtAddCallback(sip->grid,XmNcellFocusCallback,CellFocusCB,sirp);
        
        XmLGridAddColumns(sip->grid,XmHEADING,0,1);
        XmLGridAddColumns(sip->grid,XmFOOTER,0,1);
        XmLGridAddRows(sip->grid,XmHEADING,0,2);
        XmLGridSetStringsPos(sip->grid,XmHEADING,0,XmHEADING,0,
                             "Dimensions\nSelected");
        XmLGridSetStringsPos(sip->grid,XmCONTENT,0,XmHEADING,0,
                             "Start\nFinish\nStride");
        XmLGridSetStringsPos
                (sip->grid,XmHEADING,DIM_NAMES_ROW,
                 XmFOOTER,0,"Total Elements");
        XmLGridSetStringsPos
                (sip->grid,XmCONTENT,FINISH_ROW,XmFOOTER,0,"Current Shape");
        XtVaSetValues(sip->grid,
                      XmNcolumnType,XmFOOTER,
                      XmNcolumn,0,
                      XmNrowRangeStart,1,
                      XmNrowRangeEnd,2,
                      XmNcellBottomBorderType,XmBORDER_NONE,
                      XmNcellTopBorderType,XmBORDER_NONE,
                      NULL);

        sirp->coords_alloced = 0;
        sirp->start_coords = NULL;
        sirp->finish_coords = NULL;
        sirp->float_types = NULL;
	sirp->go = go;
	sirp->ignore_focus_cb = False;
        sip->edit_row = -1;
        sirp->qfileref = NrmNULLQUARK;
	sirp->qvar = NrmNULLQUARK;
	sirp->dl = NULL;
        sirp->vinfo = NULL;
        sip->index_mode = False;
        sip->headline_on = False;
        sip->highlight_on = False;
        sip->synchro_step = False;
        
        return (NgShapeInfoGrid *) sirp;
}

void NgDeactivateShapeInfoGrid
(
        NgShapeInfoGrid		*shape_info_grid
        )
{
        NgShapeInfoGridRec *sirp;
        NgShapeInfoGrid *sip;

        sirp = (NgShapeInfoGridRec *) shape_info_grid;
        if (!sirp) return;
        sip = &sirp->shapeinfogrid;
        
        if (sip->selected_dim > -1) {
                XtVaSetValues(sip->grid,
                              XmNcolumn,sip->selected_dim,
                              XmNrow,DIM_NAMES_ROW,
                              XmNrowType,XmHEADING,
                              XmNcellForeground,Foreground,
                              XmNcellBackground,Background,
                              NULL);
                XtVaSetValues(sip->grid,
                              XmNcolumn,sip->selected_dim,
                              XmNrowRangeStart,START_ROW,
                              XmNrowRangeEnd,STRIDE_ROW,
			      XmNcellBackground,sirp->go->go.edit_field_pixel,
                              XmNcellRightBorderColor,Right_Border_Color,
                              XmNcellLeftBorderColor,Left_Border_Color,
                              NULL);
        }
        sip->selected_dim = -1;
        sip->synchro_step = False;
        
        return;
}

void NgDestroyShapeInfoGrid
(
        NgShapeInfoGrid		*shape_info_grid
        )
{
        NgShapeInfoGridRec *sirp;
        
        sirp = (NgShapeInfoGridRec *) shape_info_grid;
        if (!sirp) return;
        
        NhlFree(sirp->start_coords);
        NhlFree(sirp->finish_coords);
        NhlFree(sirp->float_types);
	if (sirp->dl)
		NclFreeDataList(sirp->dl);

        NhlFree(sirp);
        
        return;
}
