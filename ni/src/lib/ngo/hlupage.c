/*
 *      $Id: hlupage.c,v 1.20 1999-01-11 19:36:25 dbrown Exp $
 */
/*******************************************x*****************************
*									*
*			     Copyright (C)  1996			*
*	     University Corporation for Atmospheric Research		*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		hlupage.c
 *
 *	Author:		David I. Brown
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Mon Jun  9 21:02:27 MDT 1997
 *
 *	Description:	
 */

#include <ncarg/ngo/hlupageP.h>
#include <ncarg/ngo/nclstate.h>
#include <ncarg/ngo/graphic.h>
#include <ncarg/ngo/xinteract.h>
#include <ncarg/ngo/stringutil.h>

#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/PushBG.h>
#include <Xm/ToggleBG.h>
#include <ncarg/hlu/App.h>
#include <ncarg/hlu/XWorkstation.h>
#include <ncarg/hlu/DataComm.h>
#include <ncarg/hlu/CoordArrays.h>
#include <ncarg/hlu/ScalarField.h>
#include <ncarg/hlu/VectorField.h>


static NrmQuark QFillValue = NrmNULLQUARK;
static NrmQuark QString = NrmNULLQUARK;
static NrmQuark QGenArray = NrmNULLQUARK;
static NrmQuark QInteger = NrmNULLQUARK;

typedef struct _ResData {
	int  res_count;
	char *res[16];
	NhlPointer values[16];
	NrmQuark types[16];
	NgVarData vdata[16];
} ResData;


static NhlBoolean
CoordItem(
       NgDataProfile  prof,
       int index
)
{
	int i;
	for (i = 0; i < abs(prof->ditems[prof->master_data_ix]->n_dims); i++) {
		if (prof->coord_items[i] == index)
			return True;
		else if (prof->coord_items[i] < 0)
			return True;
	}
	return False;
}

static void UpdateDependentVars
(
       brHluPageRec	*rec,
       int		index
)
{
       NgDataProfile	prof = rec->data_profile;
       int i;
       NgClassType match_type = prof->ditems[index]->class_type;

       for (i = 0; i < prof->n_dataitems; i++) {
	       NgDataItem ditem = prof->ditems[i];
	       if (ditem->class_type == match_type)
		       ditem->vdata->new_val = True;
       }
}
static NhlBoolean SetVarData
(
       brHluPageRec	*rec,
       NgVarData	in_var
)
{
	int		i,j,cix,var_dim_count = 0;
	NhlBoolean	dims_supplied[32];
	int 		primary_data_item = 0;
	int		last_dim = -1;
	int		max_dims;
	NclApiDataList          *dl;
	NclApiVarInfoRec	*vinfo = NULL;	

	if (! rec->data_profile) {
		NHLPERROR((NhlWARNING,NhlEUNKNOWN,
			   "internal error: data profile not set"));
                rec->has_input_data = False;
		return False;
	}
	if (! rec->data_profile->n_dataitems)
		return False;
	primary_data_item = rec->data_profile->master_data_ix;

	if (in_var->data_ix < 0)
		in_var->data_ix = primary_data_item;
	else if (in_var->data_ix != primary_data_item) {
		if (rec->data_profile->ditems[in_var->data_ix]->vdata ==
		    in_var) {
			UpdateDependentVars(rec,in_var->data_ix);
			return True; /* already set */
		}
	}
	/***************************************************************/

	if (in_var->qfile && in_var->qvar)
		dl = NclGetFileVarInfo(in_var->qfile,in_var->qvar);
	else if (in_var->qvar)	
                dl = NclGetVarInfo(in_var->qvar);

	if (dl)
		vinfo = dl->u.var;

	if (! vinfo) {
		NHLPERROR((NhlWARNING,NhlEUNKNOWN,
			   "invalid file %s or variable %s",
			   NrmQuarkToString(in_var->qfile),
			   NrmQuarkToString(in_var->qvar)));
                rec->has_input_data = False;
		return False;
	}
	for (i = 0; i < vinfo->n_dims; i++) {
		if (abs((in_var->finish[i] - in_var->start[i]) /
		    in_var->stride[i]) > 0) {
			dims_supplied[i] = True;
			var_dim_count++;
			last_dim = i;
		}
		else {
			dims_supplied[i] = False;
		}
	}
	max_dims = abs(rec->data_profile->ditems[primary_data_item]->n_dims);
	if (rec->data_profile->ditems[primary_data_item]->n_dims == 0)
		;
	else if (var_dim_count < 
		 rec->data_profile->ditems[primary_data_item]->n_dims) {
		NHLPERROR((NhlWARNING,NhlEUNKNOWN,
			   "insufficient dimensionality for %s",
			  rec->data_profile->ditems[primary_data_item]->name));
                rec->has_input_data = False;
                return False;
	}
	else if (var_dim_count > max_dims) {
		int dims_used = 0;
		for (i = vinfo->n_dims - 1; i >= 0; i--) {
			if (dims_supplied[i]) {
				if (dims_used < max_dims)
					dims_used++;
				else 
					dims_supplied[i] = False;
			}
		}
		var_dim_count = dims_used;
	}
	for (i = 0; i < rec->data_profile->n_dataitems; i++) {
		NgVarData vdata = rec->data_profile->ditems[i]->vdata;
		if (vdata->qvar == NrmNULLQUARK)
			continue;
		if (i == rec->data_profile->master_data_ix ||
		    ! CoordItem(rec->data_profile,i)) {
			if (vinfo->n_dims > vdata->dims_alloced) {
				int size = vinfo->n_dims * sizeof(int);
				vdata->start = NhlRealloc(vdata->start,size);
				vdata->finish = NhlRealloc(vdata->finish,size);
				vdata->stride = NhlRealloc(vdata->stride,size);
				vdata->dims_alloced = var_dim_count;
			}
			vdata->ndims = vinfo->n_dims;
			for (j = 0; j < vinfo->n_dims; j++) {
				if (! dims_supplied[j]) {
					vdata->finish[j] = vdata->start[j] =
						in_var->start[j];
					vdata->stride[j] = 1;
					continue;
				}
				vdata->start[j] = in_var->start[j];
				vdata->finish[j] = in_var->finish[j];
				vdata->stride[j] = in_var->stride[j];
			}
			vdata->new_val = True;
			vdata->data_ix = i;
			if (i == primary_data_item)
				vdata->dl = dl;
			vdata->type = vdata->qfile ? FILEVAR : NORMAL;
			continue;
		}
		if (last_dim < 0 ||
		    vinfo->coordnames[last_dim] <= NrmNULLQUARK)
			continue;
		var_dim_count = 1;
		if (var_dim_count > vdata->dims_alloced) {
			int size = var_dim_count * sizeof(int);
			vdata->start = NhlRealloc(vdata->start,size);
			vdata->finish = NhlRealloc(vdata->finish,size);
			vdata->stride = NhlRealloc(vdata->stride,size);
			vdata->dims_alloced = var_dim_count;
		}
		vdata->ndims = 1;
		vdata->start[0] = in_var->start[last_dim];
		vdata->finish[0] = in_var->finish[last_dim];
		vdata->stride[0] = in_var->stride[last_dim];
		vdata->new_val = True;
		if (in_var->qfile) {
			vdata->qfile = in_var->qfile;
			vdata->qvar = vinfo->coordnames[last_dim];
			vdata->type = FILEVAR;
		}
		else {
			/* 
			 * cheating here: for reading var coordinates,
			 * use qfile for the variable name, qvar for the
			 * coordinate variable
			 */
			vdata->qfile = in_var->qvar;
			vdata->qvar = vinfo->coordnames[last_dim];
			vdata->type = COORD;
		}
		vdata->data_ix = i;
		last_dim--;
		while (last_dim > -1) {
			if (dims_supplied[last_dim])
				break;
			last_dim--;
		}
	}
	UpdateDependentVars(rec,in_var->data_ix);
        rec->has_input_data = True;
	return True;
}

static void HluPageFocusNotify (
        brPage *page,
        NhlBoolean in
        )
{
        brPageData	*pdp = page->pdata;
	brHluPageRec	*rec = (brHluPageRec	*)pdp->type_rec;
        
        if (in && rec->res_tree)
                NgRestoreResTreeOverlays(rec->res_tree);
        return;
}

static void
SetValCB
(
	NhlArgVal	cbdata,
	NhlArgVal	udata
)
{
	brSetValCBInfo   *info = (brSetValCBInfo *)udata.lngval;
	brHluPageRec	*rec;

#if DEBUG_HLUPAGE
        fprintf(stderr,"in setval cb\n");
#endif
	rec = (brHluPageRec *)NgPageData(info->goid,info->pid);
        if (! rec || rec->hlu_id <= NhlNULLOBJID)
                return;
/*
 * The setvalue callback must be blocked for setvalues calls performed from
 * the hlupage itself.
 */
        if (rec->do_setval_cb)
                NgResTreeResUpdateComplete(rec->res_tree,rec->hlu_id,False);

        return;
}

static void
DestroyCB
(
	NhlArgVal	cbdata,
	NhlArgVal	udata
)
{
	brSetValCBInfo   *info = (brSetValCBInfo *)udata.lngval;
	brHluPageRec	*rec;

#if DEBUG_HLUPAGE
        fprintf(stderr,"in destroy cb\n");
#endif
	rec = (brHluPageRec *)NgPageData(info->goid,info->pid);
        if (! rec || rec->state < _hluCREATED)
                return;

	NgDeletePage(info->goid,info->pid);

        return;
}

static void GetDataVal
(
	NgVarData 	vdata,
	NhlPointer 	*value,
	NrmQuark 	*type,
	NhlBoolean 	preview
)
{
	int i;
	NclApiVarInfoRec *vinfo;
	NclExtValueRec *val = NULL;
	NhlString type_str;


	*value = NULL;
	*type = NrmNULLQUARK;

	if (! vdata)
		return;


	if (! preview) {
		char buf[1024];

		if (! vdata->qvar) {
			sprintf(buf,"null");
			*value = NhlMalloc(strlen(buf) + 1);
			strcpy((char *)*value,buf);
			*type = QString;
			return;
		}

		switch (vdata->type) {
		case FILEVAR:
			sprintf(buf,"%s->%s(",
				NrmQuarkToString(vdata->qfile),
				NrmQuarkToString(vdata->qvar));
			break;
		case COORD:
			sprintf(buf,"%s&%s(",
				NrmQuarkToString(vdata->qfile),
				NrmQuarkToString(vdata->qvar));
			break;
		case NORMAL:
			sprintf(buf,"%s(",
				NrmQuarkToString(vdata->qvar));
			break;
		default:
			fprintf(stderr,"invalid var type\n");
			return;
		}
		for (i = 0; i < vdata->ndims; i++) {
			if (abs((vdata->finish[i] - vdata->start[i]) /
				vdata->stride[i]) < 1) {
				sprintf(&buf[strlen(buf)],"%d,",
					vdata->start[i]);
				continue;
			}
	                sprintf(&buf[strlen(buf)],"%d:%d:%d,",
                        vdata->start[i],vdata->finish[i],vdata->stride[i]);
		}
		/* backing up 1 to get rid of last comma */
		sprintf(&buf[strlen(buf)-1],")");
		*value = NhlMalloc(strlen(buf) + 1);
		strcpy((char *)*value,buf);
		*type = QString;
		return;
	}
	switch (vdata->type) {
	case FILEVAR:
		val = NclReadFileVar
			(vdata->qfile,vdata->qvar,
			 vdata->start,vdata->finish,vdata->stride);
		break;
	case COORD:
		val = NclReadVarCoord
			(vdata->qfile,vdata->qvar,
			 vdata->start,vdata->finish,vdata->stride);
		break;
	case NORMAL:
		val = NclReadVar
			(vdata->qvar,
			 vdata->start,vdata->finish,vdata->stride);
		break;
	default:
		fprintf(stderr,"invalid var type\n");
		return;
	}
	if (! val)
		return;

	type_str = NgHLUTypeString(val->type);
	*value = _NhlCreateGenArray(val->value,type_str,val->elem_size,
				   val->n_dims,val->dim_sizes,!val->constant);
	NclFreeExtValue(val);
	*type = QGenArray;

	return;
}


static void GetConfigValue
(
	brHluPageRec	*rec,
	NgDataItem	ditem,
	NhlPointer 	*value,
	NrmQuark 	*type,
	NhlBoolean 	preview
)
{
	char buf[128];
	NhlString str;

	*value = NULL;
	*type = NrmNULLQUARK;

	/* this just sets the X/YCast values for now */

	if (preview)
		buf[0] = '\0';
	else
		strcpy(buf,"\"");
	if (ditem->class_type == _NgCOORDARRAY) {

		if (ditem->ref_ditem->vdata->ndims == 2) {
			strcat(buf,"MULTIPLEVECTORS");
		}
		else {
			strcat(buf,"SINGLEVECTOR");
		}
		if (! preview)
			strcat(buf,"\"");
		str = NhlMalloc(strlen(buf)+1);
		strcpy(str,buf);
		*value = (NhlPointer) str;
	}
	*type = QString;

	return;
}

static void GetFillValue
(
	brHluPageRec	*rec,
	NgDataItem	ditem,
	NhlPointer 	*value
)
{
	int i;
	NclApiVarInfoRec *vinfo;
	NclApiDataList   *dl = NULL;
        static NrmQuark QFillValue = NrmNULLQUARK;
	NclExtValueRec *val = NULL;
	char *lvalue;
	NgVarData vdata;
	NrmQuark qfile,qvar;

	*value = NULL;
	vdata = ditem->ref_ditem->vdata;
	qfile = vdata->qfile;
	qvar = vdata->qvar;
	if (!qvar)
		return;

        if (QFillValue == NrmNULLQUARK) {
                QFillValue = NrmStringToQuark("_FillValue"); 
        }
	if (vdata->dl) {
		vinfo = vdata->dl->u.var;
	}
	else {
		if (qfile > NrmNULLQUARK)
			dl = NclGetFileVarInfo(qfile,qvar);
		else	
			dl = NclGetVarInfo(qvar);
		vinfo = dl->u.var;
	}

	for (i = 0; i < vinfo->n_atts; i++) {
                if (vinfo->attnames[i] == QFillValue)
                        break;
        }
	if (i == vinfo->n_atts)
		return;

	if (qfile >  NrmNULLQUARK)
                val = NclReadFileVarAtt(qfile,qvar,QFillValue);
        else 
                val = NclReadVarAtt(qvar,QFillValue);

        if (val) {
                lvalue = (NhlPointer) NclTypeToString(val->value,val->type);
	        *value = NhlMalloc(strlen(lvalue)+1);
		strcpy(*value,lvalue);
                if (val->constant != 0)
                        NclFree(val->value);
                NclFreeExtValue(val);
        }
	if (dl)
		 NclFreeDataList(dl);
	return;
}
 
static void PreviewResList
(
        int		setrl_id,
        NhlPointer	data
        )
{
	ResData *res_data = (ResData *) data;
	int i;

	for (i = 0; i < res_data->res_count; i++) {
		if (! res_data->values[i])
			continue;
		NhlRLSet(setrl_id,res_data->res[i],
			 NrmQuarkToString(res_data->types[i]),
			 res_data->values[i]);
	}
	return;
}

static void AddResList
(
        int		nclstate,
        NhlPointer	data,
        int		block_id
        )
{
	ResData *res_data = (ResData *) data;
	NhlBoolean quote[16];
	int i,lcount = 0;
	NhlString res[16],values[16];

	for (i = 0; i < res_data->res_count; i++) {
		if (res_data->values[i]) {
			res[lcount] = res_data->res[i];
			values[lcount] = (NhlString) res_data->values[i];
			quote[lcount] = False;
			lcount++;
		}
	}

        NgNclVisBlockAddResList(nclstate,block_id,lcount,res,values,quote);

	return;
}

static Boolean ManageDataObj
(
        brPage		*page,
	NgClassType 	type,
	NgDataItem	obj_ditem,
	int		data_ix
)
{
	brPageData	*pdp = page->pdata;
	brHluPageRec	*rec = (brHluPageRec *) pdp->type_rec;
	int i, res_count = 0;
	NhlBoolean preview = rec->state == _hluNOTCREATED;
	int hlu_id;
	char *class_name;
	NhlErrorTypes ret = NhlNOERROR;
	ResData res_data;
	NgSetResProc	resproc; 
	NhlPointer	resdata;
	char *suffix;
	char buf[256];
	char *name;
	int count;

	if (obj_ditem->item_type != _NgDATAOBJ)
		return False;

	memset(res_data.res,0,16 * sizeof(char *));
	memset(res_data.values,0,16 * sizeof(NhlPointer));
	memset(res_data.types,0,16 * sizeof(NrmQuark));

	switch (type) {
	case _NgSCALARFIELD:
		class_name = "scalarFieldClass";
		suffix = "sf";
		break;
	case _NgVECTORFIELD:
		class_name = "vectorFieldClass";
		suffix = "vf";
		break;
	case _NgCOORDARRAY:
		class_name = "coordArraysClass";
		suffix = "ca";
		break;
	}

	count = 0;
	for (i=0; i < rec->data_profile->n_dataitems; i++) {
		NgDataItem ditem = rec->data_profile->ditems[i];
		NgVarData vdata;
		if (ditem->class_type != type)
			continue;
		
		vdata = ditem->vdata;
		if (! vdata->new_val)
			continue;
		res_data.res[count] = ditem->resname;
		if (ditem->item_type == _NgMISSINGVAL) {
			GetFillValue(rec,ditem,&res_data.values[count]);
			res_data.types[count] = QString;
			count++;
			continue;
		}
		else if (ditem->item_type == _NgCONFIG) {
			GetConfigValue(rec,ditem,&res_data.values[count],
				       &res_data.types[count],preview);
			count++;
			continue;
		}
			
		if (preview && ! vdata->qvar)
			continue;

		GetDataVal(vdata,&res_data.values[count],
			   &res_data.types[count],preview);
		res_data.vdata[count] = vdata;
		count++;
	}
	res_data.res_count = count;

	if (preview) {
		NgPreviewResProc	resproc;

		sprintf(buf,"%s_%s",NrmQuarkToString(page->qvar),suffix);
                name = NgNclGetSymName(rec->nclstate,buf,True);
                
                    /* create the NCL graphic variable using this name now
                       in order that it won't be "stolen" before the hlu
                       object actually gets created */
                
                sprintf(buf,"%s = new(1,graphic)\n",name);
                (void)NgNclSubmitBlock(rec->nclstate,buf);

		resproc = PreviewResList;
		resdata = (NhlPointer) &res_data;
			
		ret = NgCreatePreviewGraphic
			(rec->go->base.id,&hlu_id,name,NULL,
			 class_name,1,&resproc,&resdata);
		if (! hlu_id || ret < NhlWARNING)
			return False;
		rec->data_objects[data_ix] = NrmStringToQuark(name);
		rec->data_ids[data_ix] = hlu_id;
		rec->data_object_count = data_ix+1;

                for (i = 0; i< res_data.res_count; i++) {
                        if (res_data.values[i]) {
				if (res_data.types[i] == QGenArray)
					NhlFreeGenArray(res_data.values[i]);
				else
					NhlFree(res_data.values[i]);
			}
                }
		return True;
	}
	
	resproc = AddResList;
	resdata = (NhlPointer) &res_data;
        if (rec->state <= _hluPREVIEW || 
	    rec->data_objects[data_ix] <= NhlNULLOBJID ) {
		NgVarData vdata = obj_ditem->vdata;

		if (rec->data_objects[data_ix]) {
			name = NrmQuarkToString(rec->data_objects[data_ix]);
		} 
		else {
			sprintf(buf,"%s_%s",
				NrmQuarkToString(page->qvar),suffix);
			name = NgNclGetSymName(rec->nclstate,buf,True);
		}
		ret = NgCreateGraphic
			(rec->go->base.id,&hlu_id,name,NULL,
			 class_name,1,&resproc,&resdata);
#if DEBUG_HLUPAGE
		fprintf(stderr,"created hlu obj with id %d\n", hlu_id);
#endif        
		if (! hlu_id || ret < NhlWARNING) {
			char buf[512];
			sprintf(buf,"%s = %s@_FillValue\n",
				NrmQuarkToString(page->qvar),
				NrmQuarkToString(page->qvar));
			(void)NgNclSubmitBlock(rec->nclstate,buf);
			return ret;
		}
		vdata->qvar = NrmStringToQuark(name);
		vdata->type = HLUOBJ;
		vdata->ndims = 0;
		vdata->new_val = True;

		rec->data_ids[data_ix] = hlu_id;
		rec->data_objects[data_ix] = NrmStringToQuark(name);
		rec->data_object_count = data_ix +1;
        }
        else if (res_data.res_count) {
		ret = NgUpdateGraphic
			(rec->go->base.id,
			 NrmQuarkToString(rec->data_objects[data_ix]),
			 1,&resproc,&resdata);
		if (ret < NhlWARNING)
			return False;
	}
        for (i = 0; i< res_data.res_count; i++) {
                if (res_data.values[i]) {
                            /* these values should all be malloced strings */
                        NhlFree(res_data.values[i]);
                }
        }
        
	return True;
}


static NhlBoolean ManagePlotObj
(
        brPage	*page,
        int	wk_id
)
{
	brPageData	*pdp = page->pdata;
	brHluPageRec	*rec = (brHluPageRec *) pdp->type_rec;
	ResData 	res_data,app_res_data;
	NhlBoolean preview = rec->state == _hluNOTCREATED;
	NhlString		parent = NULL;
	int hlu_id,i;
        NgSetResProc	setresproc[2];
        NhlPointer	setresdata[2],appresdata;
	NhlErrorTypes		ret;
	int app_id = NhlNULLOBJID;
	char plotstyledir[512];
        
	char *cnres[] = {
		"cnScalarFieldData",
		NULL 
	};
        
	char *stres[] = {
		"stVectorFieldData",
		NULL
	};

	char *vcres[] = {
		"vcVectorFieldData",
		"vcScalarFieldData",
		NULL
	};
	char *xyres[] = {
		"xyCoordData",
		"xyXIrregularPoints",
		"xyYIrregularPoints",
		"xyXStyle",
		"xyYStyle",
		NULL
	};

	memset(res_data.values,0,16 * sizeof(NhlPointer));
	memset(res_data.types,0,16 * sizeof(NrmQuark));

	switch (rec->data_profile->type) {
	case _NgCONTOURPLOT:
		res_data.res_count = 1;
		for (i = 0; i < res_data.res_count; i++)
			res_data.res[i] = cnres[i];
		break;
	case _NgSTREAMLINEPLOT:
		res_data.res_count = 1;
		for (i = 0; i < res_data.res_count; i++)
			res_data.res[i] = stres[i];
		break;
	case _NgVECTORPLOT:
		res_data.res_count = 2;
		for (i = 0; i < res_data.res_count; i++)
			res_data.res[i] = vcres[i];
		break;
	case _NgXYPLOT:
		res_data.res_count = 1;
		for (i = 0; i < res_data.res_count; i++)
			res_data.res[i] = xyres[i];
		break;
	}
	if (rec->public.plot_style) {

#if DEBUG_HLUPAGE
                fprintf(stderr,"plot style %s\n",rec->public.plot_style);
#endif
		app_res_data.res_count = 1;
		app_res_data.res[0] = "appUsrDir";
                if (rec->public.plot_style_dir) {
                        strcpy(plotstyledir,"\"");
                        strcat(plotstyledir,rec->public.plot_style_dir);
                        strcat(plotstyledir,"\"");
                        app_res_data.values[0] =
                                (NhlPointer) plotstyledir;
                }
		else	
			app_res_data.values[0] = 
				(NhlPointer) "\"./plot_styles\"";
		app_res_data.types[0] = QString;
	}

	if (preview) { 
		NgPreviewResProc	resproc[2];
		NhlPointer		resdata[2];

		for (i = 0; i < res_data.res_count; i++) {
			res_data.values[i] = (NhlPointer) rec->data_ids[i];
			res_data.types[i] = QInteger;
		}
		if (rec->public.plot_style) {
			NgPreviewResProc	resproc;

			resproc = PreviewResList;
			appresdata = &app_res_data;
			ret = NgCreatePreviewGraphic
				(rec->go->base.id,&app_id,
				 rec->public.plot_style,NULL,"appClass",
				 1,&resproc,&appresdata);
			if (! app_id || ret < NhlWARNING)
				return False;
			res_data.res[res_data.res_count] = "objAppObj";
			res_data.values[res_data.res_count] =
				(NhlPointer)app_id;
			res_data.types[res_data.res_count] = QInteger;
			res_data.res_count++;
		}
		resproc[0] = PreviewResList;
		resdata[0] = &res_data;
		resproc[1] = NgResTreePreviewResList;
		resdata[1] = (NhlPointer)rec->res_tree;

		if (wk_id != NhlNULLOBJID)
			parent = NgNclGetHLURef(rec->go->go.nclstate,wk_id);
		ret = NgCreatePreviewGraphic
			(rec->go->base.id,&hlu_id,
			 NrmQuarkToString(page->qvar),parent,
			 rec->public.class_name,2,resproc,resdata);
		if (! hlu_id || ret < NhlWARNING)
			return False;
		rec->hlu_id = hlu_id;
		if (rec->public.plot_style)
			NgDestroyPreviewGraphic(rec->go->base.id,app_id);
		return True;
	}

	for (i = 0; i < res_data.res_count; i++) {
		res_data.values[i] = 
			(NhlPointer) NrmQuarkToString(rec->data_objects[i]);
		res_data.types[i] = QString;
	}
        setresproc[0] = NgResTreeAddResList;
        setresdata[0] = (NhlPointer)rec->res_tree;
        setresproc[1] = AddResList;
	setresdata[1] = (NhlPointer)&res_data;
	
        if (rec->state == _hluPREVIEW || rec->hlu_id <= NhlNULLOBJID) {
		if (rec->public.plot_style) {
			NgSetResProc	resproc;

			resproc = AddResList;
			appresdata = &app_res_data;
			ret = NgCreateGraphic
				(rec->go->base.id,&app_id,
				 rec->public.plot_style,NULL,"appClass",
				 1,&resproc,&appresdata);
			if (! app_id || ret < NhlWARNING)
				return False;

			res_data.res[res_data.res_count] = "objAppObj";
			res_data.values[res_data.res_count] =
				(NhlPointer)rec->public.plot_style;
			res_data.types[res_data.res_count] = QString;
			res_data.res_count++;
		}
		if (wk_id != NhlNULLOBJID)
			parent = NgNclGetHLURef(rec->go->go.nclstate,wk_id);

		ret = NgCreateGraphic
			(rec->go->base.id,&hlu_id,
			 NrmQuarkToString(page->qvar),parent,
			 rec->public.class_name,2,setresproc,setresdata);
#if DEBUG_HLUPAGE
		fprintf(stderr,"created hlu obj with id %d\n", hlu_id);
#endif        
		if (! hlu_id || ret < NhlWARNING) {
			char buf[512];
			sprintf(buf,"%s = %s@_FillValue\n",
				NrmQuarkToString(page->qvar),
				NrmQuarkToString(page->qvar));
			(void)NgNclSubmitBlock(rec->nclstate,buf);
			return ret;
		}
		rec->hlu_id = hlu_id;

		if (rec->public.plot_style && app_id) 
			NgDestroyGraphic(rec->go->base.id,
					 rec->public.plot_style);
        }
        else {
		int setresproc_count = rec->new_data ? 2 : 1;
		setresproc_count = 1;
		for (i =0; i< rec->data_profile->n_dataitems; i++) {
			NgDataItem ditem = rec->data_profile->ditems[i];
			if (ditem->class_type == rec->data_profile->type &&
			    ditem->vdata->new_val) {
				setresproc_count = 2;
				break;
			}
			
		}
		ret = NgUpdateGraphic
			(rec->go->base.id,NrmQuarkToString(page->qvar),
			 setresproc_count,setresproc,setresdata);
		if (ret < NhlWARNING)
			return False;
	}

	return True;
}

static int
CreatePreviewInstance
(
        brPage	*page,
        int	wk_id
)
{
        NhlErrorTypes	ret;
	brPageData	*pdp = page->pdata;
	brHluPageRec	*rec = (brHluPageRec *) pdp->type_rec;
        int		hlu_id;
        NgPreviewResProc	resproc[2];
        XtPointer	resdata[2];
        NhlString	parent = NULL;
	NgDataProfile	dprof = rec->data_profile;
	int		i,count = 0;


        if (dprof && rec->has_input_data) {
#if 0
		NgClassType type;

		switch (rec->data_profile->type) {
		case _NgCONTOURPLOT:
		case _NgSCALARFIELD:
			type = _NgSCALARFIELD;
			break;	
		case _NgSTREAMLINEPLOT:
		case _NgVECTORPLOT:
		case _NgVECTORFIELD:
			type = _NgVECTORFIELD;
			break;	
		case _NgXYPLOT:
		case _NgCOORDARRAY:
			type = _NgCOORDARRAY;
			break;	
		}
		if (! ManageDataObj(page,type,0))
			return NhlFATAL;
		rec->data_object_count++;
		
		if (dprof->type == _NgVECTORPLOT &&
		    dprof->ditems[6]->vdata->qvar != NrmNULLQUARK) {
			type = _NgSCALARFIELD;
			if (! ManageDataObj(page,type,1))
				return NhlFATAL;
			rec->data_object_count++;
		}
#endif
		for (i = 0; i < dprof->n_dataitems; i++) {
			NgDataItem ditem = dprof->ditems[i];
			if (ditem->item_type == _NgDATAOBJ &&
			    (ditem->val_defined)(dprof,i)) {
				if (! ManageDataObj
				    (page,ditem->ref_ditem->class_type,
				     ditem,count))
					return NhlFATAL;
				count++;
			}
		}
		if (! ManagePlotObj(page,wk_id))
			return NhlFATAL;
		return rec->hlu_id;
	}

	resproc[0] = NgResTreePreviewResList;
	resdata[0] = (NhlPointer)rec->res_tree;
	if (wk_id != NhlNULLOBJID)
		parent = NgNclGetHLURef(rec->go->go.nclstate,wk_id);
	ret = NgCreatePreviewGraphic
		(rec->go->base.id,&hlu_id,
		 NrmQuarkToString(page->qvar),parent,
		 rec->public.class_name,1,resproc,resdata);
        if (ret > NhlFATAL && hlu_id > NhlNULLOBJID)
                return hlu_id;
	else
		return NhlFATAL;
}

static int
CreateInstance
(
        brPage	*page,
        int	wk_id
)
{
        NhlErrorTypes	ret;
	brPageData	*pdp = page->pdata;
	brHluPageRec	*rec = (brHluPageRec *) pdp->type_rec;
        int		i,hlu_id;
        NgSetResProc	setresproc[2];
        XtPointer	setresdata[2];
        NhlString	parent = NULL;
	NgWksObj	wks = NULL;
	NgDataProfile	dprof = rec->data_profile;
	int		count = 0;

        if (dprof && rec->has_input_data) {
#if 0
		NgClassType type;

		switch (rec->data_profile->type) {
		case _NgCONTOURPLOT:
		case _NgSCALARFIELD:
			type = _NgSCALARFIELD;
			break;	
		case _NgSTREAMLINEPLOT:
		case _NgVECTORPLOT:
		case _NgVECTORFIELD:
			type = _NgVECTORFIELD;
			break;	
		case _NgXYPLOT:
		case _NgCOORDARRAY:
			type = _NgCOORDARRAY;
			break;	
		}
		if (! ManageDataObj(page,type,0))
			return NhlFATAL;
		rec->data_object_count++;
		
		if (dprof->type == _NgVECTORPLOT &&
			dprof->ditems[6]->vdata->qvar != NrmNULLQUARK) {
			type = _NgSCALARFIELD;
			if (! ManageDataObj(page,type,1))
				return NhlFATAL;
			rec->data_object_count++;
		}
#endif
		for (i = 0; i < dprof->n_dataitems; i++) {
			NgDataItem ditem = dprof->ditems[i];
			if (ditem->item_type == _NgDATAOBJ &&
			    (ditem->val_defined)(dprof,i)) {
				if (! ManageDataObj
				    (page,ditem->ref_ditem->class_type,
				     ditem,count))
					return NhlFATAL;
				count++;
			}
		}
		if (! ManagePlotObj(page,wk_id))
			return NhlFATAL;
		rec->new_data = False;
		return rec->hlu_id;
	}
        setresproc[0] = NgResTreeAddResList;
        setresdata[0] = (NhlPointer)rec->res_tree;

        if (wk_id != NhlNULLOBJID)
                parent = NgNclGetHLURef(rec->go->go.nclstate,wk_id);

        ret = NgCreateGraphic
                (rec->go->base.id,&hlu_id,
                 NrmQuarkToString(page->qvar),parent,
                 rec->public.class_name,1,setresproc,setresdata);
#if DEBUG_HLUPAGE
        fprintf(stderr,"created hlu obj with id %d\n", hlu_id);
#endif        
        if (ret < NhlWARNING) {
                char buf[512];
                sprintf(buf,"%s = %s@_FillValue\n",
                        NrmQuarkToString(page->qvar),
                        NrmQuarkToString(page->qvar));
                (void)NgNclSubmitBlock(rec->nclstate,buf);
                return ret;
        }
        return hlu_id;

/*
 * No explicit draw on create because it is handled automatically by
 * the ViewTree (mwin) create CB.
 */
}

static NhlErrorTypes
UpdateInstance
(
        brPage	*page,
        int	wk_id
)
{
        NhlErrorTypes	ret = NhlNOERROR;
	brPageData	*pdp = page->pdata;
	brHluPageRec	*rec = (brHluPageRec *) pdp->type_rec;
        NgSetResProc	setresproc[2];
        XtPointer	setresdata[2];
	NhlLayer	l = _NhlGetLayer(rec->hlu_id);
	NhlLayer	wl = _NhlGetLayer(wk_id);
	NgDataProfile	dprof = rec->data_profile;
	int		i,count = 0;

	if (! l)	
		return NhlFATAL;

        if (dprof && rec->has_input_data) {
#if 0
		NgClassType type;

		switch (rec->data_profile->type) {
		case _NgCONTOURPLOT:
		case _NgSCALARFIELD:
			type = _NgSCALARFIELD;
			break;	
		case _NgSTREAMLINEPLOT:
		case _NgVECTORPLOT:
		case _NgVECTORFIELD:
			type = _NgVECTORFIELD;
			break;	
		case _NgXYPLOT:
		case _NgCOORDARRAY:
			type = _NgCOORDARRAY;
			break;	
		}

		if (rec->new_data) {
			if (! ManageDataObj(page,type,0))
				return NhlFATAL;
		
			if (dprof->type == _NgVECTORPLOT &&
			    dprof->ditems[6]->vdata->qvar != NrmNULLQUARK) {
				type = _NgSCALARFIELD;
				if (! ManageDataObj(page,type,1))
					return NhlFATAL;
			}
		}
#endif
		for (i = 0; i < dprof->n_dataitems; i++) {
			NgDataItem ditem = dprof->ditems[i];
			if (rec->new_data &&
			    ditem->item_type == _NgDATAOBJ &&
			    (ditem->val_defined)(dprof,i)) {
				if (! ManageDataObj
				    (page,ditem->ref_ditem->class_type,
				     ditem,count))
					return NhlFATAL;
				count++;
			}
		}
		
		if (! ManagePlotObj(page,wk_id))
			return NhlFATAL;
		rec->new_data = False;
	}
        else {
                setresproc[0] = NgResTreeAddResList;
                setresdata[0] = (XtPointer)rec->res_tree;
        
                ret = NgUpdateGraphic
                        (rec->go->base.id,NrmQuarkToString(page->qvar),
                         1,setresproc,setresdata);
        }
/*
 * There is no auto callback for setvalues yet, so for updates the draw
 * must occur here
 */
	if (NhlClassIsSubclass(rec->class,NhlviewClass) &&
	    _NhlIsClass(wl,NhlxWorkstationClass)) {
		NgWksObj	wks = NULL;
		int 		draw_id = _NhlTopLevelView(rec->hlu_id);
		
		if (draw_id && wl) {
			wks = (NgWksObj) wl->base.gui_data2;
		}
		if (wks && wks->auto_refresh) {
			NgDrawXwkView(wks->wks_wrap_id,draw_id,True);
		}
	}

        return ret;
}

static void
CreateUpdate
(
        brPage		*page,
        int		wk_id
)
{
	brPageData	*pdp = page->pdata;
	brHluPageRec	*rec = (brHluPageRec *) pdp->type_rec;
        NhlBoolean 	already_created = False;
        int		i,hlu_id;
	NhlLayer	wl = _NhlGetLayer(wk_id);
	
#if DEBUG_HLUPAGE
        fprintf(stderr,"CreateUpdate -- class %s, hlu_id %d\n",
		rec->class->base_class.class_name,rec->hlu_id);
#endif

        rec->do_setval_cb = False;
        if (rec->state == _hluNOTCREATED) {
		if (! rec->class) {
			NHLPERROR((NhlWARNING,NhlEUNKNOWN,
			   "No class specified for %s: cannot create graphic",
				   NrmQuarkToString(page->qvar)));
			return;
		}
                if (NhlClassIsSubclass(rec->class,NhldataItemClass)) {
                        hlu_id = NhlNULLOBJID;
                }
                else {
                        hlu_id = CreatePreviewInstance(page,wk_id);
                        if (hlu_id <= NhlNULLOBJID) {
                                NHLPERROR((NhlFATAL,NhlEUNKNOWN,
                                           "Could not create %s graphic",
                                           NrmQuarkToString(page->qvar)));
                                rec->hlu_id = NhlNULLOBJID;
                                return;
                        }
                }
                rec->hlu_id = hlu_id;
                rec->state = _hluPREVIEW;
                rec->res_tree->preview_instance = True;
        }
        else  if (rec->state == _hluPREVIEW) {
                XmString xmstring;
                
                if (rec->hlu_id > NhlNULLOBJID)
                        NgDestroyPreviewGraphic(rec->go->base.id,rec->hlu_id);
		for (i = 0; i < rec->data_object_count; i++) {
			if (rec->data_ids[i])
				NgDestroyPreviewGraphic
					(rec->go->base.id,rec->data_ids[i]);
		}
		rec->data_object_count = 0;
                hlu_id = CreateInstance(page,wk_id);
                if (hlu_id <= NhlNULLOBJID) {
                        rec->hlu_id = NhlNULLOBJID;
                        rec->state = _hluPREVIEW;
                }
                else {
                        rec->hlu_id = hlu_id;
                        rec->state = _hluCREATED;
                
                        xmstring = NgXAppCreateXmString
                                (rec->go->go.appmgr,"Update");
                        XtVaSetValues(rec->create_update,
                                      XmNlabelString,xmstring,
                                      NULL);
                        NgXAppFreeXmString(rec->go->go.appmgr,xmstring);
                        rec->res_tree->preview_instance = False;
                }

		for (i = 0; i < rec->data_profile->n_dataitems; i++)
			rec->data_profile->ditems[i]->vdata->new_val = False; 
        }
        else {
                UpdateInstance(page,wk_id);
                already_created = True;
		for (i = 0; i < rec->data_profile->n_dataitems; i++)
			rec->data_profile->ditems[i]->vdata->new_val = False; 
        }
        if (rec->hlu_id > NhlNULLOBJID && ! already_created) {
                NhlArgVal sel,user_data;
                
                NhlINITVAR(sel);
                NhlINITVAR(user_data);
                sel.lngval = 0;
		rec->setval_info.pid = page->id;
		rec->setval_info.goid = page->go->base.id;
                user_data.ptrval = &rec->setval_info;
                rec->setval_cb = _NhlAddObjCallback
                        (_NhlGetLayer(rec->hlu_id),_NhlCBobjValueSet,
                         sel,SetValCB,user_data);
                rec->destroy_cb = _NhlAddObjCallback
                        (_NhlGetLayer(rec->hlu_id),_NhlCBobjDestroy,
                         sel,DestroyCB,user_data);
        }
        NgResTreeResUpdateComplete(rec->res_tree,rec->hlu_id,False);
	rec->do_setval_cb = True;

	if (rec->state == _hluCREATED &&
	    NhlClassIsSubclass(rec->class,NhlviewClass) &&
	    _NhlIsClass(wl,NhlxWorkstationClass)) {
		NgWksObj	wks = NULL;
		int 		draw_id = _NhlTopLevelView(rec->hlu_id);
		NhlLayer 	drawl = _NhlGetLayer(draw_id);
		
		if (drawl && wl) {
			wks = (NgWksObj) wl->base.gui_data2;
		}
		if (wks) {
			NgSetSelectedXwkView(wks->wks_wrap_id,draw_id);
		}
	}

	return;
}

int GetWorkstation
(
        brPage		*page,
        NhlBoolean	*work_created
        )
{
	brPageData	*pdp = page->pdata;
	brHluPageRec	*rec = (brHluPageRec *) pdp->type_rec;
        int wk_id;
	NhlLayer l = _NhlGetLayer(rec->hlu_id);
        
        if (! NhlClassIsSubclass(rec->class,NhlviewClass)) {
                wk_id = NhlNULLOBJID;
        }
        else if (l) {
		wk_id = l->base.wkptr->base.id;
	}
	else {
                wk_id = NgAppGetSelectedWork
                        (page->go->go.appmgr,True,work_created);
        }
        return wk_id;
}

static void CreateUpdateCB 
(
	Widget		w,
	XtPointer	udata,
	XtPointer	cb_data
)
{
        brPage		*page = (brPage *)udata;
	brPageData	*pdp = page->pdata;
	brHluPageRec	*rec = (brHluPageRec *) pdp->type_rec;
        NgHluPage 	*pub = &rec->public;
        int		wk_id;
	char		buf[512];
        NhlBoolean	work_created;

#if DEBUG_HLUPAGE
        fprintf(stderr,"in CreateUpdateCB\n");
#endif
        
        wk_id = GetWorkstation(page,&work_created);
	CreateUpdate(page,wk_id);

        return;
}

static void AutoUpdateCB 
(
	Widget		w,
	XtPointer	udata,
	XtPointer	cb_data
)
{
        brPage		*page = (brPage *)udata;
	brPageData	*pdp = page->pdata;
	brHluPageRec	*rec = (brHluPageRec *) pdp->type_rec;
        Boolean		set;

#if DEBUG_HLUPAGE
        fprintf(stderr,"in AutoUpdateCB\n");
#endif

        XtVaGetValues(w,
                      XmNset,&set,
                      NULL);
        
        rec->do_auto_update = set;
        
        return;
}
#if 0
static void
CopyVarData
(
        NgVarDataRec **to_data,
        NgVarDataRec *from_data
        )
{
        if (! from_data) {
                if (*to_data) {
                        if ((*to_data)->start)
                                NhlFree((*to_data)->start);
                        if ((*to_data)->finish)
                                NhlFree((*to_data)->finish);
                        if ((*to_data)->stride)
                                NhlFree((*to_data)->stride);
                        NhlFree((*to_data));
                }
                (*to_data) = NULL;
                return;
        }
        if (! *to_data) {
                (*to_data) = NhlMalloc(sizeof(NgVarDataRec));
                (*to_data)->ndims = (*to_data)->dims_alloced = 0;
                (*to_data)->start = NULL;
                (*to_data)->finish = NULL;
                (*to_data)->stride = NULL;
        }
        if (from_data->ndims > (*to_data)->dims_alloced) {
                (*to_data)->start = NhlRealloc
                        ((*to_data)->start,from_data->ndims * sizeof(long));
                (*to_data)->finish = NhlRealloc
                        ((*to_data)->finish,from_data->ndims * sizeof(long));
                (*to_data)->stride = NhlRealloc
                        ((*to_data)->stride,from_data->ndims * sizeof(long));
		(*to_data)->dims_alloced = from_data->ndims;
        }
	
	(*to_data)->type = from_data->type;
        (*to_data)->qfile = from_data->qfile;
        (*to_data)->qvar = from_data->qvar;
        (*to_data)->ndims = from_data->ndims;
        (*to_data)->data_ix = from_data->data_ix;

	if (! from_data->dl) {
		(*to_data)->dl = NULL;
	}
	else {
		NclApiDataList          *dl;
		if (from_data->qfile)
			dl = NclGetFileVarInfo(from_data->qfile,
					       from_data->qvar);
		else	
			dl = NclGetVarInfo(from_data->qvar);
		(*to_data)->dl = dl;
	}

        memcpy((*to_data)->start,
               from_data->start,(*to_data)->ndims * sizeof(long));
        memcpy((*to_data)->finish,
               from_data->finish,(*to_data)->ndims * sizeof(long));
        memcpy((*to_data)->stride,
               from_data->stride,(*to_data)->ndims * sizeof(long));

        return;
}
#endif

static void HluPageInputNotify (
        brPage *page,
        brPageType output_page_type,
 	NhlPointer output_data
        )
{
        brPageData	*pdp = page->pdata;
	brHluPageRec	*rec = (brHluPageRec	*)pdp->type_rec;
        NgHluPage 	*pub = &rec->public;
        NgVarDataRec	*var_data = (NgVarDataRec *)output_data;
        int		wk_id;
        NhlBoolean 	work_created,non_contour = False;
        
        
#if DEBUG_HLUPAGE
        fprintf(stderr,"in hlu page input notify\n");
#endif
        switch (output_page_type) {
            case _brNULL:
                    non_contour = True;
                    break;
            case _brREGVAR:
            case _brFILEVAR:
		    if (! SetVarData(rec,var_data))
			    return;
                    NgUpdateDataSourceGrid
                            (rec->data_source_grid,
			     page->qvar,rec->data_profile);
		    rec->has_input_data = True;
                    rec->new_data = True;
                    break;
            default:
		    NHLPERROR((NhlFATAL,NhlEUNKNOWN,
			       "page type not supported for input"));
		    return;
        }

        if (rec->state == _hluNOTCREATED || rec->do_auto_update) {
                
                wk_id = GetWorkstation(page,&work_created);
                
		CreateUpdate(page,wk_id);

        }
        
        return;
}

static NhlPointer PublicHluPageData (
        brPage *page
        )
{
        brPageData	*pdp = page->pdata;
	brHluPageRec	*rec = (brHluPageRec	*)pdp->type_rec;
        NgHluPage 	*pub = &rec->public;
        
#if DEBUG_HLUPAGE
        fprintf(stderr,"in public hlu page data\n");
#endif
        
        return (NhlPointer) pub;
}
static void SetValuesCB 
(
	Widget		w,
	XtPointer	udata,
	XtPointer	cb_data
)
{
        brPage		*page = (brPage *)udata;
	brPageData	*pdp = page->pdata;
	brHluPageRec	*rec = (brHluPageRec *) pdp->type_rec;

#if	0
	NgResTreeSetValues(rec->res_tree);
#endif
	return;
}
static void
DeactivateHluPage
(
	brPage	*page
)
{
	brHluPageRec *rec = (brHluPageRec *)page->pdata->type_rec;
        int i;
        
        if (rec->create_update)
                XtRemoveCallback(rec->create_update,
                                 XmNactivateCallback,CreateUpdateCB,page);
        if (rec->auto_update)
                XtRemoveCallback(rec->auto_update,
                                 XmNvalueChangedCallback,AutoUpdateCB,page);

        if (rec->setval_cb) {
               _NhlCBDelete(rec->setval_cb);
               rec->setval_cb = NULL;
        }
        if (rec->destroy_cb) {
               _NhlCBDelete(rec->destroy_cb);
               rec->destroy_cb = NULL;
        }
        
        rec->state = _hluNOTCREATED;
        rec->hlu_id = NhlNULLOBJID;
        rec->do_auto_update = False;
        rec->public.class_name = NULL;
        rec->public.plot_style = NULL;
	rec->public.plot_style_dir = NULL;
        rec->new_data = True;
        rec->has_input_data = False;

	if (rec->res_tree) {
		XtUnmanageChild(rec->res_tree->tree);
	}
        
        for (i=0; i <  8; i++) {
                rec->data_objects[i] = NrmNULLQUARK;
		rec->data_ids[i] = NhlNULLOBJID;
	}
	NgFreeDataProfile(rec->data_profile);
	rec->data_profile = rec->public.data_profile = NULL;
	rec->data_object_count = 0;
        rec->activated = False;
}

static void DestroyHluPage
(
	NhlPointer data
)
{
	brHluPageRec	*hlu_rec = (brHluPageRec *)data;
        int i,j;


        if (hlu_rec->setval_cb) {
               _NhlCBDelete(hlu_rec->setval_cb);
               hlu_rec->setval_cb = NULL;
        }
        if (hlu_rec->destroy_cb) {
               _NhlCBDelete(hlu_rec->destroy_cb);
               hlu_rec->destroy_cb = NULL;
        }
        
        NgDestroyDataSourceGrid(hlu_rec->data_source_grid);
	NgFreeDataProfile(hlu_rec->data_profile);
        NgDestroyResTree(hlu_rec->res_tree);
        
        NhlFree(data);
	return;
}

static void
AdjustHluPageGeometry
(
        NhlPointer	data
)
{
	brPage	*page = (brPage *) data;
        brHluPageRec	*rec;
	Dimension		w,h,y,twidth,theight;
        Dimension		avail_width,avail_height;
        
	rec = (brHluPageRec *)page->pdata->type_rec;
        
	twidth = 0;
	theight = 0;
        w = 0;
        y = 0;
        h = 0;
        if (rec->data_source_grid && 
	    XtIsManaged(rec->data_source_grid->grid)) {
                XtVaGetValues(rec->data_source_grid->grid,
                              XmNwidth,&w,
                              XmNy,&y,
                              XmNheight,&h,
                              NULL);
        }
	twidth = w;
        
        if (rec->res_tree)
                XtVaGetValues(rec->res_tree->tree,
                              XmNwidth,&w,
                              XmNy,&y,
                              XmNheight,&h,
                              NULL);
        
	twidth = MAX(w,twidth);
        theight = y + h;
        
        NgSetFolderSize(page->pdata->pane,
                        twidth,theight,&avail_width,&avail_height);
	
	return;
}
        
static NhlErrorTypes UpdateHluPage
(
        brPage *page
        )
{
        brPageData	*pdp = page->pdata;
	brHluPageRec	*rec = (brHluPageRec	*)pdp->type_rec;
        NgHluPage 	*pub = &rec->public;
	int		hlu_id,i;
        
#if DEBUG_HLUPAGE
        fprintf(stderr,"in updata hlu page\n");
#endif

        if (! pub->class_name) {
               NHLPERROR((NhlFATAL,NhlEUNKNOWN,
                          "no class specified for graphic variable %s",
                          NrmQuarkToString(page->qvar)));
                return NhlFATAL;
        } 
                
#if DEBUG_HLUPAGE
        fprintf(stderr,"%s\n",pub->class_name);
#endif

        if (rec->hlu_id > NhlNULLOBJID)
                rec->class = NhlClassOfObject(rec->hlu_id);
        else
                rec->class = NgNclHluClassPtrFromName(rec->nclstate,
                                                      pub->class_name);
        
        if (!rec->class) {
                NHLPERROR((NhlFATAL,NhlEUNKNOWN,
                           "%s is not a user instantiable class",
                           pub->class_name));
                return NhlFATAL;
        }

	if (! (NhlClassIsSubclass(rec->class,NhldataCommClass) ||
		NhlClassIsSubclass(rec->class,NhldataItemClass))) {
		if (XtIsManaged(rec->data_source_grid->grid))
			XtUnmanageChild(rec->data_source_grid->grid);
	}
	if (! pub->data_profile) {
		NhlString name;
		name = NgHasDataProfile(rec->go,pub->class_name) ?
			pub->class_name : NULL;
		pub->data_profile = NgGetDataProfile(rec->go,name);
		if (! pub->data_profile) {
			NHLPERROR((NhlFATAL,NhlEUNKNOWN,
				   "error getting data profile for %s",
				   pub->class_name));
			return NhlFATAL;
		}
	}
		
	rec->data_profile = pub->data_profile;
	if (rec->data_profile->n_dataitems) {
		NgUpdateDataSourceGrid
			(rec->data_source_grid,page->qvar,rec->data_profile);
		if (! XtIsManaged(rec->data_source_grid->grid))
			XtManageChild(rec->data_source_grid->grid);
	}

/* ResTree */
        
        if (!rec->res_tree) {
                rec->res_tree = NgCreateResTree
                                (page->go,pdp->form,page->qvar,
                                 rec->class,rec->hlu_id);
                XtVaSetValues(rec->res_tree->tree,
                              XmNrightAttachment,XmATTACH_NONE,
                              XmNbottomAttachment,XmATTACH_NONE,
                              XmNtopOffset,8,
                              XmNtopAttachment,XmATTACH_WIDGET,
                              XmNtopWidget,rec->create_update,
                              NULL);
                rec->res_tree->geo_notify = AdjustHluPageGeometry;
        }
        else {
                rec->res_tree->preview_instance =
                        rec->state == _hluCREATED ? False : True;
		XtManageChild(rec->res_tree->tree);
                NgUpdateResTree
                        (rec->res_tree,page->qvar,rec->class,rec->hlu_id);
        }
        XtVaGetValues(pdp->pane->scroller,
                      XmNhorizontalScrollBar,&rec->res_tree->h_scroll,
                      XmNverticalScrollBar,&rec->res_tree->v_scroll,
                      NULL);
        
        rec->res_tree->geo_data = (NhlPointer) page;
        
        return NhlNOERROR;

}

static brPageData *
NewHluPage
(
  	NgGO		go,
        brPane		*pane,
	brPage		*page
        )
{
	brPageData	*pdp;
	brHluPageRec	*rec;
        NhlString	e_text;
        int		i;

	if (!(pdp = NhlMalloc(sizeof(brPageData)))) {
		e_text = "%s: dynamic memory allocation error";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,_NhlName(go));
                return NULL;
	}
        pdp->dl = NULL;
	pdp->next = pane->hlu_pages;
	pane->hlu_pages = pdp;

	rec = (brHluPageRec*) NhlMalloc(sizeof(brHluPageRec));
	if (! rec) {
		e_text = "%s: dynamic memory allocation error";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,_NhlName(go));
                NhlFree(pdp);
                return NULL;
	}
        pdp->type_rec = (NhlPointer) rec;
        rec->go = go;
        
        NhlVAGetValues(rec->go->go.appmgr,
                       NgNappNclState,	&rec->nclstate,
                       NULL);
        
        rec->data_source_grid = NULL;
        rec->res_tree = NULL;
        rec->create_update = NULL;
        rec->auto_update = NULL;
        rec->new_data = True;
        rec->has_input_data = False;
        rec->state = _hluNOTCREATED;
        rec->do_auto_update = False;
        rec->public.data_profile = NULL;
        rec->public.class_name = NULL;
        rec->public.plot_style = NULL;
        rec->public.plot_style_dir = NULL;
        rec->class = NULL;
        rec->hlu_id = NhlNULLOBJID;
        rec->setval_cb = NULL;
        rec->destroy_cb = NULL;
        rec->do_setval_cb = False;
	rec->data_profile = NULL;
        
	rec->data_object_count = 0;
        for (i=0; i <  8; i++) {
                rec->data_objects[i] = NrmNULLQUARK;
		rec->data_ids[i] = NhlNULLOBJID;
	}

	pdp->form = XtVaCreateManagedWidget
		("form",xmFormWidgetClass,pane->folder,
                 XmNy,28,
                 XmNx,2,
                 NULL);
        
	pdp->destroy_page = DestroyHluPage;
	pdp->adjust_page_geo = AdjustHluPageGeometry;
	pdp->deactivate_page = DeactivateHluPage;
	pdp->page_output_notify = NULL;
        pdp->page_input_notify = HluPageInputNotify;
        pdp->public_page_data = PublicHluPageData;
        pdp->update_page = UpdateHluPage;
        pdp->page_focus_notify = HluPageFocusNotify;
        pdp->pane = pane;
        
        rec->data_source_grid = NgCreateDataSourceGrid
                (rec->go,pdp->form,page->qvar,rec->data_profile);
        XtVaSetValues(rec->data_source_grid->grid,
                      XmNbottomAttachment,XmATTACH_NONE,
                      XmNrightAttachment,XmATTACH_NONE,
                      NULL);
        
        rec->create_update = XtVaCreateManagedWidget
                ("Create/Update",xmPushButtonGadgetClass,pdp->form,
                 XmNtopAttachment,XmATTACH_WIDGET,
                 XmNtopWidget,rec->data_source_grid->grid,
                 XmNrightAttachment,XmATTACH_NONE,
                 XmNbottomAttachment,XmATTACH_NONE,
                 NULL);

        rec->auto_update = XtVaCreateManagedWidget
                ("Auto Update",xmToggleButtonGadgetClass,pdp->form,
                 XmNtopAttachment,XmATTACH_WIDGET,
                 XmNtopWidget,rec->data_source_grid->grid,
                 XmNleftAttachment,XmATTACH_WIDGET,
                 XmNleftWidget,rec->create_update,
                 XmNrightAttachment,XmATTACH_NONE,
                 XmNbottomAttachment,XmATTACH_NONE,
                 NULL);
        
        return pdp;
}
        
extern brPageData *
NgGetHluPage
(
	NgGO		go,
        brPane		*pane,
	brPage		*page,
        brPage		*copy_page
)
{
	NgBrowse		browse = (NgBrowse)go;
	NgBrowsePart		*np = &browse->browse;
        NhlString		e_text;
	brPageData		*pdp;
	brHluPageRec		*rec,*copy_rec = NULL;
        NrmQuark		*qhlus;
        NhlBoolean		is_hlu = False;
        int			i;
        static int		first = True;
	int			hlu_id,count,*hlu_array = NULL;
	int			nclstate;
        XmString		xmstring;

        if (QFillValue == NrmNULLQUARK) {
                QFillValue = NrmStringToQuark("_FillValue"); 
		QString = NrmStringToQuark(NhlTString);
		QGenArray = NrmStringToQuark(NhlTGenArray);
		QInteger = NrmStringToQuark(NhlTInteger);
        }

	NhlVAGetValues(go->go.appmgr,
		NgNappNclState,	&nclstate,
		NULL);

	hlu_id = NgNclGetHluObjId
                (nclstate,NrmQuarkToString(page->qvar),&count,&hlu_array);
	if (hlu_id < NhlNOERROR)
		return NULL;

	if (count > 1) {
		NHLPERROR((NhlWARNING,NhlEUNKNOWN,
                       "variable %s is an array: only handling first element",
                           NrmQuarkToString(page->qvar)));
		NhlFree(hlu_array);
	}

	if (copy_page) {
		copy_rec = (brHluPageRec *) copy_page->pdata->type_rec;
	}
	for (pdp = pane->hlu_pages; pdp != NULL; pdp = pdp->next) {
		if (!pdp->in_use)
		  break;
	}
        if (! pdp)
                pdp = NewHluPage(go,pane,page);
        if (! pdp)
                return NULL;
        page->pdata = pdp;
	pdp->in_use = True;
        
        rec = (brHluPageRec *) pdp->type_rec;
        
	if (hlu_id > NhlNULLOBJID && 
	    ((rec->class = NhlClassOfObject(hlu_id)) != NULL)) {
                NhlArgVal sel,user_data;
                
		rec->hlu_id = hlu_id;
		rec->public.class_name = rec->class->base_class.class_name;
                if (copy_rec && copy_rec->state == _hluPREVIEW)
                        rec->state = _hluPREVIEW;
                else
                        rec->state = _hluCREATED;
                NhlINITVAR(sel);
                NhlINITVAR(user_data);
                sel.lngval = 0;
		rec->setval_info.pid = page->id;
		rec->setval_info.goid = page->go->base.id;
                user_data.ptrval = &rec->setval_info;
                rec->setval_cb = _NhlAddObjCallback
                        (_NhlGetLayer(hlu_id),_NhlCBobjValueSet,
                         sel,SetValCB,user_data);
                rec->destroy_cb = _NhlAddObjCallback
                        (_NhlGetLayer(rec->hlu_id),_NhlCBobjDestroy,
                         sel,DestroyCB,user_data);
	}
        else {
                rec->hlu_id = NhlNULLOBJID;
                rec->class = NULL;
                rec->public.class_name = NULL;
                rec->state = _hluNOTCREATED;
        }
        if (rec->state == _hluCREATED)
                xmstring = NgXAppCreateXmString(rec->go->go.appmgr,"Update");
        else
                xmstring = NgXAppCreateXmString(rec->go->go.appmgr,"Create");
        XtVaSetValues(rec->create_update,
                      XmNlabelString,xmstring,
                      NULL);
        NgXAppFreeXmString(rec->go->go.appmgr,xmstring);

        rec->activated = True;
        XtAddCallback
                (rec->create_update,XmNactivateCallback,CreateUpdateCB,page);
        XtAddCallback
                (rec->auto_update,XmNvalueChangedCallback,AutoUpdateCB,page);
        

        if (! copy_page) {
                if (rec->hlu_id > NhlNULLOBJID)
                        UpdateHluPage(page);
                return pdp;
        }

	rec->public.data_profile = NgCopyDataProfile
		(copy_rec->public.data_profile);
	rec->data_profile = rec->public.data_profile;
        rec->class = copy_rec->class;
        rec->public.class_name = copy_rec->public.class_name;
        rec->public.plot_style = copy_rec->public.plot_style;
        rec->public.plot_style_dir = copy_rec->public.plot_style_dir;
        rec->hlu_id = copy_rec->hlu_id;
        rec->state = copy_rec->state;
	rec->new_data = copy_rec->new_data;
	rec->has_input_data = copy_rec->has_input_data;
        rec->do_auto_update = copy_rec->do_auto_update;
        
/* ResTree */
        
        if (rec->res_tree) {
		NgDupResTree(page->go,pdp->form,page->qvar,
			     copy_rec->class,copy_rec->hlu_id,
			     rec->res_tree,copy_rec->res_tree);
        }
        else {
                rec->res_tree = NgDupResTree
                        (page->go,pdp->form,page->qvar,
                         copy_rec->class,copy_rec->hlu_id,
                         NULL,copy_rec->res_tree);
                XtVaSetValues(rec->res_tree->tree,
                              XmNrightAttachment,XmATTACH_NONE,
                              XmNbottomAttachment,XmATTACH_NONE,
                              XmNtopOffset,8,
                              XmNtopAttachment,XmATTACH_WIDGET,
                              XmNtopWidget,rec->create_update,
                              NULL);
                rec->res_tree->geo_notify = AdjustHluPageGeometry;
        }
        XtVaGetValues(pdp->pane->scroller,
                      XmNhorizontalScrollBar,&rec->res_tree->h_scroll,
                      XmNverticalScrollBar,&rec->res_tree->v_scroll,
                      NULL);
        rec->res_tree->geo_data = (NhlPointer) page;
        rec->res_tree->preview_instance =
                (rec->state == _hluCREATED) ? False : True;


        if (rec->do_auto_update)
                XtVaSetValues(rec->auto_update,
                              XmNset,True,
                              NULL);
	if (! (rec->class && 
	       (NhlClassIsSubclass(rec->class,NhldataCommClass) ||
		NhlClassIsSubclass(rec->class,NhldataItemClass)))) {
		if (XtIsManaged(rec->data_source_grid->grid))
			XtUnmanageChild(rec->data_source_grid->grid);
	}
	else {
		rec->data_profile = NgCopyDataProfile(copy_rec->data_profile);
		if (rec->data_profile->n_dataitems) {
			NgUpdateDataSourceGrid
				(rec->data_source_grid,
				 page->qvar,rec->data_profile);
			if (! XtIsManaged(rec->data_source_grid->grid))
				XtManageChild(rec->data_source_grid->grid);
		}
		if (copy_rec->data_object_count) {
			for (i = 0; i < copy_rec->data_object_count; i++) {
				rec->data_objects[i] = 
					copy_rec->data_objects[i];
				rec->data_ids[i] = copy_rec->data_ids[i];
			}
			rec->data_object_count = copy_rec->data_object_count;
		}
	}
        return pdp;
}
