/*
 *      $Id: nclstate.c,v 1.23 1999-11-19 02:10:07 dbrown Exp $
 */
/************************************************************************
*									*
*			     Copyright (C)  1996			*
*	     University Corporation for Atmospheric Research		*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		nclstate.c
 *
 *	Author:		Jeff W. Boote
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Fri Aug 30 10:53:24 MDT 1996
 *
 *	Description:	
 */
#include <ncarg/ngo/nclstateP.h>
#include <ncarg/ngo/app.h>

#include <ncarg/hlu/ErrorI.h>

#include <ncarg/ncl/defs.h>
#include <ncarg/ncl/NclDataDefs.h>
#include <ncarg/ncl/ApiRecords.h>
#include <ncarg/ncl/NclApi.h>
#include <ncarg/ncl/NclCallBacksI.h>

#define	Oset(field)	NhlOffset(NgNclStateRec,nclstate.field)
static NhlResource resources[] = {
	{NgNnsHluClassCount,NgCnsHluClassCount,NhlTInteger,sizeof(int),
         Oset(classcount),NhlTImmediate,_NhlUSET((NhlPointer)NULL),
         _NhlRES_GONLY,NULL},
	{NgNnsHluClasses,NgCnsHluClasses,NhlTPointer,sizeof(NhlPointer),
         Oset(classlist),NhlTImmediate,_NhlUSET((NhlPointer)NULL),
         _NhlRES_GONLY,NULL},
};

static NrmQuark QHlu_Class_Count;
static NrmQuark QHlu_Classes;

static _NhlRawObjCB callbacks[] = {
	{NgCBnsObject,Oset(object),3,NULL,NULL},
	{NgCBnsOutput,Oset(outputcb),0,NULL,NULL},
	{NgCBnsErrOutput,Oset(erroutputcb),0,NULL,NULL},
	{NgCBnsSubmit,Oset(submitcb),0,NULL,NULL},
	{NgCBnsPrompt,Oset(promptcb),0,NULL,NULL},
	{NgCBnsReset,Oset(resetcb),0,NULL,NULL},
	{NgCBnsPostSubmit,Oset(post_submitcb),0,NULL,NULL},
};
#undef	Oset

static NhlErrorTypes NclStateClassPartInitialize(
	NhlClass	lc
);

static NhlErrorTypes NclStateClassInitialize(
	void
);

static NhlErrorTypes NclStateInitialize(
	NhlClass	lc,
	NhlLayer	req,
	NhlLayer	new,
	_NhlArgList	args,
	int		nargs
);

static NhlErrorTypes NclStateDestroy(
	NhlLayer	l
);

NgNclStateClassRec NgnclStateClassRec = {
	{
/* class_name			*/	"nclStateClass",
/* nrm_class			*/	NrmNULLQUARK,
/* layer_size			*/	sizeof(NgNclStateRec),
/* class_inited			*/	False,
/* superclass			*/	(NhlClass)&NhlobjClassRec,
/* cvt_table			*/	NULL,

/* layer_resources		*/	resources,
/* num_resources		*/	NhlNumber(resources),
/* all_resources		*/	NULL,
/* callbacks			*/	callbacks,
/* num_callbacks		*/	NhlNumber(callbacks),
/* class_callbacks	*/	NULL,
/* num_class_callbacks	*/	0,

/* class_part_initialize	*/	NclStateClassPartInitialize,
/* class_initialize		*/	NclStateClassInitialize,
/* layer_initialize		*/	NclStateInitialize,
/* layer_set_values		*/	NULL,
/* layer_set_values_hook	*/	NULL,
/* layer_get_values		*/	NULL,
/* layer_reparent		*/	NULL,
/* layer_destroy		*/	NclStateDestroy,

	},
	{
/* foo				*/	0,
	}
};

NhlClass NgnclStateClass = (NhlClass)&NgnclStateClassRec;

/*
 * Function:	NclStateClassPartInitialize
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
static NhlErrorTypes
NclStateClassPartInitialize
(
	NhlClass	lc
)
{
	/*
	 * Don't allow sub-classes!
	 */
	if(lc != NgnclStateClass)
		return NhlFATAL;

	NgnclStateClassRec.nclstate_class.num = 0;
	QHlu_Class_Count = NrmStringToQuark(NgNnsHluClassCount);
	QHlu_Classes = NrmStringToQuark(NgNnsHluClasses);
        
	return NhlNOERROR;
}

/*
 * Function:	NclStateClassInitialize
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
static NhlErrorTypes
NclStateClassInitialize
(
	void
)
{

	return NhlNOERROR;
}

static NgNclAny
InsertObj
(
	NgNclAny	*tree,
	NgNclAny	parent,
	NgNclAny	node
)
{
	NgNclObj	*next;

	if(!node)
		return NULL;

	if(!*tree){
		*tree = node;

		node->parent = (NgNclObj)parent;
		return node;
	}

	if((*tree)->id == node->id){
		(*tree)->ref_count++;
		NhlFree(node);
		return NULL;
	}

	if(node->id < (*tree)->id)
		next = &(*tree)->left;
	else
		next = &(*tree)->right;

	return InsertObj((NgNclAny*)next,*tree,node);
}

static NgNclAny
ExtractObj
(
	NgNclAny	*tree,
	NgNclAny	node
)
{
	NgNclAny	tmp;
	NgNclObj	*next;

	if(!*tree){
		NHLPERROR((NhlFATAL,NhlEUNKNOWN,NULL));
		NhlFree(node);
		return NULL;
	}

	if((*tree)->id == node->id){
		if((*tree)->ref_count){
			(*tree)->ref_count--;
			NhlFree(node);
			return NULL;
		}
		tmp = *tree;
		*tree = (NgNclAny)tmp->left;
		(void)InsertObj(tree,(NgNclAny)tmp->parent,
							(NgNclAny)tmp->right);
		tmp->cbtype = node->cbtype;
		NhlFree(node);
		return tmp;
	}

	if(node->id < (*tree)->id)
		next = &(*tree)->left;
	else
		next = &(*tree)->right;

	return ExtractObj((NgNclAny*)next,node);
}

static NhlBoolean
ProcessObj
(
	NgNclState	ncl,
	NgNclObj	*list,
	NgNclObj	*tree,
	int		id
)
{
	char		func[]="ProcessObj";
	NgNclAny	*alist = (NgNclAny*)list;
	NgNclAny	*atree = (NgNclAny*)tree;
	NgNclAny	node=NULL;
	NhlArgVal	cbtype,objptr;
	NhlBoolean	dofree=False;
	NgNclCBType	deltype=NgNclCBUNKNOWN;

	while(*alist){
		if((*alist)->id == id){
			node = *alist;
			*alist = (NgNclAny)node->next;
			break;
		}
		alist = (NgNclAny*)&(*alist)->next;
	}

	if(!node){
		NHLPERROR((NhlWARNING,NhlEUNKNOWN,
						"Can't find id %d in queue?"));
		return False;
	}

	switch(node->cbtype){
		
		case NgNclCBCREATE_HLUOBJ:
		case NgNclCBCREATE_HLUVAR:
		case NgNclCBCREATE_VAR:
		case NgNclCBCREATE_FILEVAR:

			/*
			 * Before adding this node into nclstate, make
			 * sure it isn't immediately destroyed by checking
			 * the list for a destroy for the same object.
			 */
			deltype = (NgNclCBType)(node->cbtype+NgNclCB_C_D_DIFF);
			alist = (NgNclAny*)list;
			while(*alist){
				if(((*alist)->cbtype == deltype) &&
						((*alist)->id == id)){
					/*
					 * Free "Create" node.
					 */
					NhlFree(node);

					/*
					 * Pull "Delete" node off of list
					 * and free it.
					 */
					node = *alist;
					*alist = (NgNclAny)node->next;
					NhlFree(node);

					return True;
				}
				alist = (NgNclAny*)&(*alist)->next;
			}

			node = InsertObj(atree,NULL,node);

			break;

		case NgNclCBDELETE_HLUOBJ:
		case NgNclCBDELETE_HLUVAR:
		case NgNclCBDELETE_VAR:
		case NgNclCBDELETE_FILEVAR:

			node = ExtractObj(atree,node);
			dofree=True;

			break;

		default:
			NHLPERROR((NhlFATAL,NhlEUNKNOWN,
						"%s:Invalid cbtype???",func));
			return False;
	}

	if(!node)
		return False;

	NhlINITVAR(cbtype);
	NhlINITVAR(objptr);
	cbtype.lngval = node->cbtype;
	objptr.ptrval = node;
	_NhlCallObjCallbacks((NhlLayer)ncl,NgCBnsObject,cbtype,objptr);

	if(dofree)
		NhlFree(node);

	return True;
}

static NhlBoolean
ObjUpdates
(
	NhlPointer	cdata
)
{
	NgNclState	ncl = (NgNclState)cdata;
	NgNclStatePart	*ns = &ncl->nclstate;
	int		id;

	if(ns->hluobj_pending){
		id = ((NgNclAny)ns->hluobj_pending)->id;
		(void)ProcessObj(ncl,&ns->hluobj_pending,&ns->hluobj_tree,id);
	}
	else if(ns->hluvar_pending){
		id = ((NgNclAny)ns->hluvar_pending)->id;
		(void)ProcessObj(ncl,&ns->hluvar_pending,&ns->hluvar_tree,id);
	}
	else if(ns->var_pending){
		id = ((NgNclAny)ns->var_pending)->id;
		(void)ProcessObj(ncl,&ns->var_pending,&ns->var_tree,id);
	}
	else if(ns->filevar_pending){
		id = ((NgNclAny)ns->filevar_pending)->id;
		(void)ProcessObj(ncl,&ns->filevar_pending,&ns->filevar_tree,id);
	}

	ns->obj_pending =	ns->hluobj_pending ||
				ns->hluvar_pending ||
				ns->var_pending ||
				ns->filevar_pending;

	return !ns->obj_pending;
}

static void
ObjPending
(
	NgNclState	ncl
)
{
	if(ncl->nclstate.obj_pending)
		return;
	ncl->nclstate.obj_pending = True;
	NgAddPriorityWorkProc(ncl->nclstate.appmgr,ObjUpdates,ncl);

	return;
}

static NgNclObj
AllocNclObj
(
	void
)
{
	NgNclObj	tmp = NhlMalloc(sizeof(NgNclObjRec));
	NgNclAny	any = (NgNclAny)tmp;

	if(!tmp)
		return NULL;

	any->cbtype = NgNclCBUNKNOWN;
	any->next = any->left = any->right = any->parent = NULL;
	any->name = NULL;
	any->id = 0;
	any->ref_count = 0;

	return tmp;
}

static NhlErrorTypes
HluObjCB
(
	NgNclCBType	cbtype,
	NhlPointer	cbdata,
	NhlPointer	udata
)
{
	NclHLUObjClassInfo	*objinfo = (NclHLUObjClassInfo*)cbdata;
	NgNclState		ncl = (NgNclState)udata;
	NgNclHluObj		node = (NgNclHluObj)AllocNclObj();
	NgNclAny		*list =(NgNclAny*)&ncl->nclstate.hluobj_pending;

	if(!node){
		NHLPERROR((NhlFATAL,ENOMEM,NULL));
		return NhlFATAL;
	}

	node->cbtype = cbtype;
	node->name = (Const char*)NrmQuarkToString(objinfo->hluobj.hlu_name);
	node->id = objinfo->hluobj.hlu_id;
	node->parent_id = objinfo->hluobj.parent_hluobj_id;
	node->class_ptr = objinfo->hluobj.class_ptr;
	node->vname = NULL;

	while(*list)
		list = (NgNclAny*)&(*list)->next;
	*list = (NgNclAny)node;

	ObjPending(ncl);

	return NhlNOERROR;
}

static NhlErrorTypes
VarCB
(
	NgNclCBType	cbtype,
	NgNclObj	*list,
	NhlPointer	cbdata,
	NhlPointer	udata
)
{
	NclVarClassInfo	*objinfo = (NclVarClassInfo*)cbdata;
	NgNclState	ncl = (NgNclState)udata;
	NgNclVar	node;
	NgNclAny	*alist = (NgNclAny*)list;

	if(objinfo->var.var_type != NclAPINORMAL)
		return NhlNOERROR;

	if(objinfo->var.var_quark < 1){
		NHLPERROR((NhlFATAL,NhlEUNKNOWN,"New Variable has NO name???"));
		return NhlFATAL;
	}

	node = (NgNclVar)AllocNclObj();
	if(!node){
		NHLPERROR((NhlFATAL,ENOMEM,NULL));
		return NhlFATAL;
	}

	node->cbtype = cbtype;
	node->name = (Const char*)NrmQuarkToString(objinfo->var.var_quark);
	node->id = objinfo->var.var_quark;
	node->scalar = ((objinfo->var.n_dims == 1) &&
					(objinfo->var.dim_sizes[0] == 1));

	while(*alist)
		alist = (NgNclAny*)&(*alist)->next;
	*alist = (NgNclAny)node;

	ObjPending(ncl);

	return NhlNOERROR;
}

static NhlErrorTypes
CreateHluObjCB
(
	NhlPointer	cbdata,
	NhlPointer	udata
)
{
	return HluObjCB(NgNclCBCREATE_HLUOBJ,cbdata,udata);
}

static NhlErrorTypes
DeleteHluObjCB
(
	NhlPointer	cbdata,
	NhlPointer	udata
)
{
	return HluObjCB(NgNclCBDELETE_HLUOBJ,cbdata,udata);
}

NhlErrorTypes
CreateHluVarCB
(
	NhlPointer	cbdata,
	NhlPointer	udata
)
{
	NgNclState	ncl = (NgNclState)udata;

	return VarCB(NgNclCBCREATE_HLUVAR,&ncl->nclstate.hluvar_pending,cbdata,
								udata);
}

NhlErrorTypes
DeleteHluVarCB
(
	NhlPointer	cbdata,
	NhlPointer	udata
)
{
	NgNclState	ncl = (NgNclState)udata;

	return VarCB(NgNclCBDELETE_HLUVAR,&ncl->nclstate.hluvar_pending,cbdata,
								udata);
}

NhlErrorTypes
CreateVarCB
(
	NhlPointer	cbdata,
	NhlPointer	udata
)
{
	NgNclState	ncl = (NgNclState)udata;

	return VarCB(NgNclCBCREATE_VAR,&ncl->nclstate.var_pending,cbdata,udata);
}

NhlErrorTypes
DeleteVarCB
(
	NhlPointer	cbdata,
	NhlPointer	udata
)
{
	NgNclState	ncl = (NgNclState)udata;

	return VarCB(NgNclCBDELETE_VAR,&ncl->nclstate.var_pending,cbdata,udata);
}

NhlErrorTypes
CreateFileVarCB
(
	NhlPointer	cbdata,
	NhlPointer	udata
)
{
	NgNclState	ncl = (NgNclState)udata;

	return VarCB(NgNclCBCREATE_FILEVAR,&ncl->nclstate.filevar_pending,
								cbdata,udata);
}

NhlErrorTypes
DeleteFileVarCB
(
	NhlPointer	cbdata,
	NhlPointer	udata
)
{
	NgNclState	ncl = (NgNclState)udata;

	return VarCB(NgNclCBDELETE_FILEVAR,&ncl->nclstate.filevar_pending,
								cbdata,udata);
}

static void
InsertFunc
(
	NgNclState		ncl,
	NgNclFunc		*tree,
	NgNclFunc		parent,
	Const char		*key,
	NclApiFuncInfoRec	*finfo
)
{
	NgNclObj	*next;

	/*
	 * if *tree is null, then insert node here.
	 */
	if(!*tree){
		NhlArgVal	cbtype,objptr;

		NhlINITVAR(cbtype);
		NhlINITVAR(objptr);

		*tree = (NgNclFunc)AllocNclObj();
		if(!*tree){
			NHLPERROR((NhlFATAL,ENOMEM,NULL));
			return;
		}

		/*
		 * Eventually fill with ARG info
		 */
		(*tree)->cbtype = NgNclCBCREATE_FUNC;

		(*tree)->next = NULL;
		(*tree)->left = NULL;
		(*tree)->right = NULL;
		(*tree)->parent = (NgNclObj)parent;

		(*tree)->name = key;
		(*tree)->id = finfo->name;

		cbtype.lngval = NgNclCBCREATE_FUNC;
		objptr.ptrval = *tree;
		_NhlCallObjCallbacks((NhlLayer)ncl,NgCBnsObject,cbtype,objptr);
		(*tree)->cbtype = NgNclFUNC;

		return;
	}

	if((*tree)->id == finfo->name)
		return;

	if(strcmp(key,(*tree)->name) < 0)
		next = &(*tree)->left;
	else
		next = &(*tree)->right;

	InsertFunc(ncl,(NgNclFunc*)next,*tree,key,finfo);

	return;
}

static NhlBoolean
UpdateFuncList
(
	NhlPointer	udata
)
{
	NgNclState	ncl = (NgNclState)udata;
	NclApiDataList	*tmp,*save;
	Const char	*key;

	if(!ncl->nclstate.func_pending)
		return True;

	tmp = save = NclGetProcFuncList();
	while(tmp != NULL){
		key = (Const char*)NrmQuarkToString(tmp->u.func->name);
		InsertFunc(ncl,(NgNclFunc*)&ncl->nclstate.func_tree,NULL,
							key,tmp->u.func);
		tmp = tmp->next;
	}
	NclFreeDataList(save);

	ncl->nclstate.func_pending = False;

	return True;
}

static void
FuncPending
(
	NgNclState	ncl
)
{
	if(ncl->nclstate.func_pending)
		return;
	ncl->nclstate.func_pending = True;
	NgAddPriorityWorkProc(ncl->nclstate.appmgr,UpdateFuncList,ncl);

	return;
}

static void
PromptFunc
(
	NhlPointer	udata,
	int		line
)
{
	*(int*)udata = line;
}

/*
 * Function:	NclOutput
 *
 * Description:	Called by ncl to print output.  The *fp is actually a pointer
 *		to the nclstate object.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
static int
NclOutput
(
	FILE		*fp,
	const char	*fmt,
	va_list		ap
)
{
	NgNclState	ncl = (NgNclState)fp;
	char		buffer[2048];
	int		len;
	NhlArgVal	dummy,cbdata;

	NhlINITVAR(dummy);
	NhlINITVAR(cbdata);

	len = vsprintf(buffer,fmt,ap);
/*
 * Braindead SunOS doesn't do vsprintf correctly!
 */
#if	defined(SunOS) && (MAJOR == 4)
	len = strlen(buffer) + 1;
#endif

	if((len == 2) && !strcmp(buffer,"\n\n"))
		buffer[1] = '\0';
	cbdata.strval = buffer;
	_NhlCallObjCallbacks((NhlLayer)ncl,NgCBnsOutput,dummy,cbdata);

	return len;
}

static void
ErrOutput
(
	NhlArgVal	cbdata,
	NhlArgVal	udata
)
{
	NgNclState	ncl = (NgNclState)udata.ptrval;
	NhlArgVal	dummy;

	ncl->nclstate.err = True;

	NhlINITVAR(dummy);
	_NhlCallObjCallbacks((NhlLayer)ncl,NgCBnsErrOutput,dummy,cbdata);

	return;
}
static int ClassSort
(
        const void *p1,
        const void *p2
)
{
        return (strcmp((*(NhlClass*)p1)->base_class.class_name,
                       (*(NhlClass*)p2)->base_class.class_name));
}

/*
 * Function:	NclStateInitialize
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
static NhlErrorTypes
NclStateInitialize
(
	NhlClass	lc,
	NhlLayer	req,
	NhlLayer	new,
	_NhlArgList	args,
	int		nargs
)
{
	char		func[] = "NclStateInitialize";
	NgNclState	nncl = (NgNclState)new;
	NgNclStatePart	*ns = &nncl->nclstate;
	NgNclStateClass	nsc = (NgNclStateClass)lc;
	NhlArgVal	dummy,udata;

	if(nsc->nclstate_class.num){
		NHLPERROR((NhlFATAL,NhlEUNKNOWN,
				"%s:Only one ns object can exist!",func));
		return NhlFATAL;
	}
	nsc->nclstate_class.num++;

	if(NhlIsClass((int)new->base.gui_data,NgappMgrClass))
		ns->appmgr = (int)new->base.gui_data;
	else{
		NHLPERROR((NhlFATAL,NhlEUNKNOWN,"%s:Invalid appmgr",func));
		return NhlFATAL;
	}

	NhlVASetValues(ns->appmgr,
		NgNappNclState,	new->base.id,
		NULL);

	NhlINITVAR(dummy);
	NhlINITVAR(udata);
	udata.ptrval = nncl;
	ns->appdestroy_cb = _NhlAddObjCallback(_NhlGetLayer(ns->appmgr),
				_NhlCBobjDestroy,dummy,NgDestroyMeCB,udata);


	ns->istate = True;
	ns->defining_proc = False;
	ns->err = False;

	ns->obj_pending = False;

	ns->hluobj_pending = NULL;
	ns->hluvar_pending = NULL;
	ns->var_pending = NULL;
	ns->filevar_pending = NULL;

	ns->func_pending = True;

	ns->hluobj_tree = NULL;
	ns->hluvar_tree = NULL;
	ns->var_tree = NULL;
	ns->filevar_tree = NULL;

	ns->func_tree = NULL;

	ns->post_submit_installed = False;

	NclSetPromptFunc(PromptFunc,&ns->line);
	(void)NclInitServer();
	NclSetOutputStream((FILE*)nncl);
	_NclSetPrintFunc(NclOutput);

	NclApiRegisterCallback
		(NCLHLUObj,NclAPICREATED,(void*)CreateHluObjCB,nncl);
	NclApiRegisterCallback
		(NCLHLUObj,NclAPIDESTROYED,(void*)DeleteHluObjCB,nncl);

	NclApiRegisterCallback
		(NCLHLUVar,NclAPICREATED,(void*)CreateHluVarCB,nncl);
	NclApiRegisterCallback
		(NCLHLUVar,NclAPIDESTROYED,(void*)DeleteHluVarCB,nncl);
	NclApiRegisterCallback
		(NCLVar,NclAPICREATED,(void*)CreateVarCB,nncl);
	NclApiRegisterCallback
		(NCLVar,NclAPIDESTROYED,(void*)DeleteVarCB,nncl);
	NclApiRegisterCallback
		(NCLFileVar,NclAPICREATED,(void*)CreateFileVarCB,nncl);
	NclApiRegisterCallback
		(NCLFileVar,NclAPIDESTROYED,(void*)DeleteFileVarCB,nncl);

	udata.ptrval = nncl;
	ns->perror_cb = _NhlAddObjCallback(_NhlGetLayer(NhlErrGetID()),
				_NhlCBerrPError,dummy,ErrOutput,udata);


	(void)UpdateFuncList((NhlPointer)nncl);
        
        ns->classlist = NclGetHLUClassPtrs(&ns->classcount);
        qsort(ns->classlist,ns->classcount,sizeof(NhlClass),ClassSort);
        
        ns->block_id = 0;
        ns->bufsize = 0;
        ns->buffer = NULL;
        
	return NhlNOERROR;
}
#if 0
static NhlErrorTypes    NclStateGetValues(
        NhlLayer	l,
        _NhlArgList     args,
        int             num_args
)

{
	NgNclState	ncl = (NgNclState)l;
	NgNclStatePart	*ns = &ncl->nclstate;
        int i;
        NhlBoolean got_class_ptrs = False;
        
        for (i = 0; i < num_args; i++) {
                if (! got_class_ptrs &&
                    (args[i].quark == QHlu_Class_Count ||
                     args[i].quark == QHlu_Classes)) {
                        ns->classlist = NclGetHLUClassPtrs(&ns->classcount);
                        qsort(ns->classlist,
                              ns->classcount,sizeof(NhlClass),ClassSort);
                        got_class_ptrs = True;
                }
                if (args[i].quark == QHlu_Class_Count) {
                        *((int*)(args[i].value.ptrval)) = ns->classcount;
                }
                else if (args[i].quark == QHlu_Classes) {
                         *((NhlPointer*)(args[i].value.ptrval))
                                 = ns->classlist;
                }
        }
        return NhlNOERROR;
}
                
#endif
static void
DestroyObjList
(
	NgNclObj	list
)
{
	NgNclAny	any = (NgNclAny)list;

	if(!any)
		return;

	DestroyObjList(any->next);
	NhlFree(any);

	return;
}

static void
DestroyObjTree
(
	NgNclObj	tree
)
{
	NgNclAny	any = (NgNclAny)tree;

	if(!any)
		return;

	DestroyObjTree(any->left);
	DestroyObjTree(any->right);
	NhlFree(any);

	return;
}

/*
 * Function:	NclStateDestroy
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
static NhlErrorTypes
NclStateDestroy
(
	NhlLayer	l
)
{
	NgNclStatePart	*ncl = &((NgNclState)l)->nclstate;

	if(ncl->obj_pending)
		NgRemoveWorkProc(ncl->appmgr,ObjUpdates,l);
	if(ncl->func_pending)
		NgRemoveWorkProc(ncl->appmgr,UpdateFuncList,l);

	_NhlCBDelete(ncl->appdestroy_cb);
	_NhlCBDelete(ncl->perror_cb);

#if 0        
	DestroyObjList(ncl->hluobj_pending);
	DestroyObjList(ncl->hluvar_pending);
	DestroyObjList(ncl->var_pending);
	DestroyObjList(ncl->filevar_pending);

	DestroyObjTree(ncl->hluobj_tree);
	DestroyObjTree(ncl->hluvar_tree);
	DestroyObjTree(ncl->var_tree);
	DestroyObjTree(ncl->filevar_tree);
	DestroyObjTree(ncl->func_tree);

	_NhlCBDestroy(ncl->object);
	_NhlCBDestroy(ncl->outputcb);
	_NhlCBDestroy(ncl->erroutputcb);
	_NhlCBDestroy(ncl->submitcb);
	_NhlCBDestroy(ncl->promptcb);
	_NhlCBDestroy(ncl->resetcb);
#endif        

	return NhlNOERROR;
}

/*
 * Public API funcion definitions
 */

NhlBoolean
NgNclProcessObj
(
	int		ncl_id,
	NgNclCBType	obj_type,
	int		id
)
{
	char		func[]="NgProcessObj";
	NgNclState	ncl = (NgNclState)_NhlGetLayer(ncl_id);

	if(!ncl || !_NhlIsClass((NhlLayer)ncl,NgnclStateClass)){
		NHLPERROR((NhlFATAL,NhlEUNKNOWN,"%s:Invalid nclstate id",func));
		return False;
	}

	switch(obj_type){
		case NgNclHLUOBJ:
			return ProcessObj(ncl,&ncl->nclstate.hluobj_pending,
						&ncl->nclstate.hluobj_tree,id);
		case NgNclHLUVAR:
			return ProcessObj(ncl,&ncl->nclstate.hluvar_pending,
						&ncl->nclstate.hluvar_tree,id);
		case NgNclVAR:
			return ProcessObj(ncl,&ncl->nclstate.var_pending,
						&ncl->nclstate.var_tree,id);
		case NgNclFILEVAR:
			return ProcessObj(ncl,&ncl->nclstate.filevar_pending,
						&ncl->nclstate.filevar_tree,id);
		default:
			NHLPERROR((NhlFATAL,NhlEUNKNOWN,
					"%s:Unsupported obj type",func));
			return False;
	}
}

static void
ResetNcl
(
	NgNclState	ncl
)
{
	NhlArgVal	dummy;
	if(ncl->nclstate.istate)
		return;

	NclResetServer();
	NhlINITVAR(dummy);
	_NhlCallObjCallbacks((NhlLayer)ncl,NgCBnsReset,dummy,dummy);

	return;
}

void
NgNclReset
(
	int	nclstate
)
{
	char		func[] = "NgNclReset";
	NgNclState	ncl = (NgNclState)_NhlGetLayer(nclstate);

	if(!ncl || !_NhlIsClass((NhlLayer)ncl,NgnclStateClass)){
		NHLPERROR((NhlFATAL,NhlEUNKNOWN,"%s:Invalid nclstate id",func));
		return;
	}

	ResetNcl(ncl);

	return;
}

NhlBoolean
SubmitNclLine
(
	NgNclState	ncl,
	char		*command
)
{
	char			func[]="SubmitNclLine";
	NgNclStatePart		*ns = &ncl->nclstate;
	char			cmd_stack[128];
	char			*cmd_buff;
	char			*end;
	int			len;
	unsigned int		size;
	NhlArgVal		dummy,cbdata;
	NgNclPromptCBDataRec	prompt;

	NhlINITVAR(dummy);
	NhlINITVAR(cbdata);

	/*
	 * Find first unescaped newline - everything after that will be
	 * ignored.
	 */
	end = strchr(command,'\n');
	while(end && end > command && *(end-1) == '\\')
		end = strchr(++end,'\n');

	if(end)
		len = end-command;
	else{
		NHLPERROR((NhlFATAL,NhlEUNKNOWN,"%s:Incomplete line (no \\n)",
									func));
		return False;
	}

	size = sizeof(char) * (len + 2);
	cmd_buff = _NgStackAlloc(size,cmd_stack);
	if(!cmd_buff){
		NHLPERROR((NhlFATAL,ENOMEM,NULL));
		return False;
	}
	strncpy(cmd_buff,command,len);
	end = cmd_buff + len;
	*end++ = '\n';
	*end = '\0';

	cbdata.strval = cmd_buff;
	_NhlCallObjCallbacks((NhlLayer)ncl,NgCBnsSubmit,dummy,cbdata);

	ns->err = False;
	len = ns->line;

	ns->istate = NclSubmitCommand(cmd_buff);

	/*
	 * Determine if the cmd just submitted could have caused a procedure
	 * to be defined.
	 */
	if(!ns->istate && (strstr(cmd_buff,"function") ||
				strstr(cmd_buff,"procedure") ||
				strstr(cmd_buff,"load")))
		ns->defining_proc = True;
	if(ns->istate && ns->defining_proc){
		ns->defining_proc = False;
		FuncPending(ncl);
	}
	else if(strstr(cmd_buff,"load"))
		FuncPending(ncl);

	_NgStackFree((NhlPointer)cmd_buff,cmd_stack);

	if(len == ns->line){
		NHLPERROR((NhlWARNING,NhlEUNKNOWN,
				"%s:Ncl didn't increment line number?",func));
	}
	prompt.line = ns->line;
	prompt.istate = ns->istate;
	prompt.err = ns->err;
	cbdata.ptrval = &prompt;
	_NhlCallObjCallbacks((NhlLayer)ncl,NgCBnsPrompt,dummy,cbdata);

	return !ns->err;
}

static NhlBoolean
PostSubmit
(
	NhlPointer	cdata
)
{
	NgNclState	ncl = (NgNclState)cdata;
	NhlArgVal	dummy;

	NhlINITVAR(dummy);

	_NhlCallObjCallbacks((NhlLayer)ncl,NgCBnsPostSubmit,dummy,dummy);
	ncl->nclstate.post_submit_installed = False;

	return True;
}

static void
SubmitPostSubmit(
	NgNclState	ncl
)
{
	if(ncl->nclstate.post_submit_installed)
		return;
	ncl->nclstate.post_submit_installed = True;
	NgAddWorkProc(ncl->nclstate.appmgr,PostSubmit,ncl);

	return;
}

/*
 * Function:	NgNclSubmitLine
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
NhlBoolean
NgNclSubmitLine
(
	int		nclstate,
	char		*command,
	NhlBoolean	reset
)
{
	char		func[] = "NgNclSubmitLine";
	NgNclState	ncl = (NgNclState)_NhlGetLayer(nclstate);

	if(!ncl || !_NhlIsClass((NhlLayer)ncl,NgnclStateClass)){
		NHLPERROR((NhlFATAL,NhlEUNKNOWN,"%s:Invalid nclstate id",func));
		return False;
	}

	if(reset)
		ResetNcl(ncl);

	SubmitPostSubmit(ncl);

	return SubmitNclLine(ncl,command);
}

/*
 * Function:	NgNclSubmitBlock
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
NhlBoolean
NgNclSubmitBlock
(
	int		nclstate,
	char		*command
)
{
	char		func[] = "NgNclSubmitBlock";
	NgNclState	ncl = (NgNclState)_NhlGetLayer(nclstate);
	NgNclStatePart	*ns;
	char		blk_stack[1024];
	char		*blk_buff,*end,*cmd;
	unsigned int	size;

	if(!ncl || !_NhlIsClass((NhlLayer)ncl,NgnclStateClass)){
		NHLPERROR((NhlFATAL,NhlEUNKNOWN,"%s:Invalid nclstate id",func));
		return False;
	}

	ns = &ncl->nclstate;

	if(!ns->istate){
		NHLPERROR((NhlFATAL,NhlEUNKNOWN,"%s:Ncl must be in init state",
									func));
		return False;
	}

	size = sizeof(char) * (strlen(command)+1);
	blk_buff = _NgStackAlloc(size,blk_stack);
	if(!blk_buff){
		NHLPERROR((NhlFATAL,ENOMEM,NULL));
		return False;
	}
	strcpy(blk_buff,command);

	/*
	 * find last unescaped newline - everything after is ignored.
	 */
	end = strrchr(blk_buff,'\n');
	while(end && (end > blk_buff)){
		if(*(end-1) != '\\')
			break;
		*end = '\0';
		end = strrchr(blk_buff,'\n');
	}
	if(!end){
		NHLPERROR((NhlWARNING,NhlEUNKNOWN,"%s:empty block",func));
		return False;
	}
	*(end+1) = '\0';

	if(SubmitNclLine(ncl,"begin\n")){
		cmd = blk_buff;
		while(*cmd){
			if(!SubmitNclLine(ncl,cmd))
				break;
			cmd = strchr(cmd,'\n');
			while(cmd && (cmd != blk_buff) && (*(cmd-1) == '\\')){
				cmd++;
				cmd = strchr(cmd,'\n');
			}
			if(!cmd) break;
			cmd++;
		}
#if 0
		if(!ncl->nclstate.err)
#endif
			SubmitNclLine(ncl,"end\n");
	}

	ResetNcl(ncl);
	_NgStackFree((NhlPointer)blk_buff,blk_stack);

	SubmitPostSubmit(ncl);

	return !ncl->nclstate.err;
}

/*
 * Function:	NgNclCurrentLine
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
NhlBoolean
NgNclCurrentLine
(
	int		nclstate
)
{
	char		func[] = "NgNclCurrentLine";
	NgNclState	ncl = (NgNclState)_NhlGetLayer(nclstate);
	NgNclStatePart	*ns;

	if(!ncl || !_NhlIsClass((NhlLayer)ncl,NgnclStateClass)){
		NHLPERROR((NhlFATAL,NhlEUNKNOWN,"%s:Invalid nclstate id",func));
		return False;
	}
	ns = &ncl->nclstate;

	return ns->line;
}

static void
EnumObj
(
	NgNclObj	node,
	NgNclEnumFunc	func,
	NhlPointer	udata
)
{
	NgNclAny	any = (NgNclAny)node;

	if(!any)
		return;

	(*func)(node,udata);
	EnumObj(any->left,func,udata);
	EnumObj(any->right,func,udata);

	return;
}

void
NgNclEnumerateObj
(
	int		ncl_id,
	NgNclCBType	obj_type,
	NgNclEnumFunc	enumerate,
	NhlPointer	udata
)
{
	char		func[]="NgNclEnumerateObj";
	NgNclState	ncl = (NgNclState)_NhlGetLayer(ncl_id);

	if(!ncl || !_NhlIsClass((NhlLayer)ncl,NgnclStateClass)){
		NHLPERROR((NhlFATAL,NhlEUNKNOWN,"%s:Invalid nclstate id",func));
		return;
	}

	switch(obj_type){
		case NgNclHLUOBJ:
			EnumObj(ncl->nclstate.hluobj_tree,enumerate,udata);
			break;
		case NgNclHLUVAR:
			EnumObj(ncl->nclstate.hluvar_tree,enumerate,udata);
			break;
		case NgNclVAR:
			EnumObj(ncl->nclstate.var_tree,enumerate,udata);
			break;
		case NgNclFILEVAR:
			EnumObj(ncl->nclstate.filevar_tree,enumerate,udata);
			break;
		case NgNclFUNC:
			EnumObj(ncl->nclstate.func_tree,enumerate,udata);
			break;
		default:
			NHLPERROR((NhlFATAL,NhlEUNKNOWN,
					"%s:Unsupported obj type",func));
			break;
	}

	return;
}

NhlString
NgNclGetSymName
(
	int		nclstate,
	NhlString	basename,
        NhlBoolean	add_zero
)
{
	static char	buff[512];
	int		i=0;

        if (add_zero)
                sprintf(buff,"%s%d",basename,i);
        else
                sprintf(buff,"%s",basename);
        
        for(; NclSymbolDefined(buff); ++i,sprintf(buff,"%s%d",basename,i));
        
	return buff;
}


/*
 * Function:	NgNclGetHLURef
 *
 * Description:	
 *		returns a string that can be used as a reference to
 *		the given hlu object in ncl.  NULL is returned if
 *		a reference can't be found.  The string is a pointer
 *		to internal memory, and should be copied immediately.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
NhlString
NgNclGetHLURef(
	int	nclstate,
	int	hluid
)
{
	char			func[]="NgNclGetHLURef";
	static char		buff[NCL_MAX_STRING*3];
	char			*ptr;
	NclHLUStruct		nhs;
	struct _NclApiDataList	*nvi;
	NclApiVarInfoRec	*vinfo;
	int			i,mult;

	if((NclHLUStringRef(hluid,&nhs)< NhlNOERROR) || (nhs.var_quark<=0))
		return NULL;

	strcpy(buff,NrmQuarkToString(nhs.var_quark));
	ptr = buff + strlen(buff);

	if(nhs.att_quark>0){
		sprintf(ptr,"@%s",NrmQuarkToString(nhs.att_quark));
		ptr += strlen(ptr);
		if(nhs.n_offsets>0)
			sprintf(ptr,"(%d)",nhs.offset);
		return buff;
	}

	nvi = NclGetVarInfo(nhs.var_quark);
#if 0        
	if(!nvi || (nvi->u.var->type != HLUOBJ)){
#endif        
	if(!nvi || (nvi->u.var->data_type != NCLAPI_obj)){
		NHLPERROR((NhlFATAL,NhlEUNKNOWN,"%s:Var Info mis-match",func));
		NclFreeDataList(nvi);
		return NULL;
	}

	vinfo = nvi->u.var;

	/*
	 * Assume scalar if n_dims is 0 - although ncl currently returns
	 * 1 dim of length 1.
	 */
	if((vinfo->n_dims == 0) ||
		((vinfo->n_dims == 1) && (vinfo->dim_info[0].dim_size == 1))){
		NclFreeDataList(nvi);
		if(nhs.offset == 0)
			return buff;
		NHLPERROR((NhlFATAL,NhlEUNKNOWN,"%s:Var Dimension mis-match",
									func));
		return NULL;
	}

	strcpy(ptr,"(");
	ptr++;

	mult=1;
	for(i=0;i<vinfo->n_dims;i++)
		mult *= vinfo->dim_info[i].dim_size;

	for(i=0;i<vinfo->n_dims;i++){
		mult /= vinfo->dim_info[i].dim_size;
		sprintf(ptr,"%d",nhs.offset/mult);
		if(i == (vinfo->n_dims-1))
			strcat(ptr,")");
		else
			strcat(ptr,",");
		ptr += strlen(ptr);
		nhs.offset %= mult;
	}
	NclFreeDataList(nvi);

	return buff;
}

/*
 * Function:	NgNclGetHluObjId
 *
 * Description:	
 *        (1) If variable not defined or it's not a graphic var, a
 *        warning is issued and an error is returned.
 *        (2) If var is defined but does not represent an instantiated 
 *        hlu object, NhlNULLOBJID is returned and but count is 1
 *        (3) If var is an array, the first member is returned as a 
 *        scalar; all members (including first) are returned in an allocated 
 *        array. The caller is responsible for freeing this array. Each 
 *        member that does not represent an instantiated hlu object is set 
 *        to NhlNULLOBJID.
 *        (4) If var is scalar it is returned and the array will be NULL.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	memory possibly allocated
 */

int
NgNclGetHluObjId(
	int		nclstate,
	NhlString	hlu_varname,
        int		*count,
        int		**id_array
        )
{
	char		func[] = "NgNclGetHluObjId";
        NclExtValueRec	*val;
        int		i, scalar_id;

        *count = 0;
        *id_array = NULL;
        
        val = NclGetHLUObjId(hlu_varname);
        if (! val) {
                NHLPERROR((NhlWARNING,NhlEUNKNOWN,
                           "%s: variable %s not defined or not graphic",
                           func,hlu_varname));
                return NhlWARNING;
        }
        
        scalar_id = ((int *)val->value)[0];
	if (val->has_missing && scalar_id == val->missing.objval)
		scalar_id = NhlNULLOBJID;
        
        *count = val->totalelements;
        if (val->totalelements == 1) {
                if (val->constant != 0)
                        NclFree(val->value);
                NclFreeExtValue(val);
                return scalar_id;
        }

	*id_array = NhlMalloc(sizeof(int) * val->totalelements);
	memcpy((char*)*id_array,(char*)val->value,
	       sizeof(int) * val->totalelements);
	for (i = 0; i < val->totalelements; i++)
		if ((*id_array)[i] == val->missing.objval)
			(*id_array)[i] = NhlNULLOBJID;

        if (val->constant != 0) {
		NclFree(val->value);
	}
	NclFreeExtValue(val);
        
        return scalar_id;
}
#if 0
int
NgNclGetHluSymbols(
        int		nclstate,
        NgGraphicType	type,
        NrmQuark	**qsymbols,
        NhlBoolean	count_only
        )
{
	NgNclState	ncl = (NgNclState)_NhlGetLayer(nclstate);
	NgNclStatePart	*ns;
        NclApiDataList	*dlist;
        NclApiHLUVarInfoRec *vinfo;
        NclApiHLUObjInfoRec *hinfo;
        int		hlu_count;
        int		i,type_count;

        *qsymbols = NULL;

        dlist = NclGetHLUObjsList();
        vi
        for (i = 0; i < count; i++) {
                int *id_array = NULL;
                int hlu_id = NgNclGetHluObjId();
        }
                
                
}
#endif

NhlClass
NgNclHluClassPtrFromName(
        int		nclstate,
	NhlString	hlu_classname
        )
{
	NgNclState	ncl = (NgNclState)_NhlGetLayer(nclstate);
	NgNclStatePart	*ns;

        int i;

        if (! ncl) return NULL;
        
        ns = &ncl->nclstate;
        for (i = 0; i < ns->classcount; i++) {
                if (! strcmp(hlu_classname,
                             ns->classlist[i]->base_class.class_name))
                        return ns->classlist[i];
        }
        return NULL;
}

/*
 * hlu_name, ncl_parent, and classname may all be set to NULL unless the
 * type is _NgCREATE.
 */

int
NgNclVisBlockBegin
(
        int		nclstate,
        NgNclBlockType	type,
	NhlString	hlu_name,
        NhlString	ncl_graphic,
        NhlString	ncl_parent,
        NhlString	classname
        )
{
	NgNclState	ncl = (NgNclState)_NhlGetLayer(nclstate);
	NgNclStatePart	*ns;

        if (! ncl) {
                NHLPERROR((NhlFATAL,NhlEUNKNOWN,"Invalid nclstate id"));
                return NhlFATAL;
        }
        
        ns = &ncl->nclstate;
        ns->block_id++;
        ns->block_type = type;

        if (! ns->buffer) {
                ns->buffer = NhlMalloc(512);
                if (!ns->buffer) {
                        NHLPERROR((NhlFATAL,ENOMEM,NULL));
                        return (int) NhlFATAL;
                }
                ns->bufsize = 512;
        }
        
        switch (type) {
            case _NgCREATE:
                    sprintf(ns->buffer,"%s = create \"%s\" %s %s\n",
                            ncl_graphic,hlu_name,classname,ncl_parent);
                    break;
            case _NgSETVAL:
                    sprintf(ns->buffer,"setvalues %s\n",
                            ncl_graphic);
                    break;
            case _NgGETVAL:
                    sprintf(ns->buffer,"getvalues %s\n",
                            ncl_graphic);
                    break;
        }

        return ns->block_id;
}

NhlErrorTypes
NgNclVisBlockEnd
(
        int		nclstate,
        int		block_id
        )
{
	NgNclState	ncl = (NgNclState)_NhlGetLayer(nclstate);
	NgNclStatePart	*ns;
        char		*endstr;

        if (! ncl) {
                NHLPERROR((NhlFATAL,NhlEUNKNOWN,"Invalid nclstate id"));
                return NhlFATAL;
        }
 
        ns = &ncl->nclstate;

        if (block_id != ns->block_id) {
                NHLPERROR((NhlFATAL,NhlEUNKNOWN,"Ncl block sequence error"));
                return NhlFATAL;
        }
        
        switch (ns->block_type) {
            case _NgCREATE:
                    endstr = "end create\n";
                    break;
            case _NgSETVAL:
                    endstr = "end setvalues\n";
                    break;
            case _NgGETVAL:
                    endstr = "end getvalues\n";
                    break;
        }

        if (strlen(ns->buffer) + strlen(endstr) > ns->bufsize) {
                ns->bufsize += 512;
                ns->buffer = NhlRealloc(ns->buffer,ns->bufsize);
                if (!ns->buffer) {
                        NHLPERROR((NhlFATAL,ENOMEM,NULL));
                        return NhlFATAL;
                }
        }
        strcat(ns->buffer,endstr);
        NgNclSubmitBlock(nclstate,ns->buffer);

        return NhlNOERROR;
}

NhlErrorTypes
NgNclVisBlockAddResList
(
        int		nclstate,
        int		block_id,
        int		res_count,
        NhlString	*res_names,
        NhlString	*values,
        NhlBoolean	*quote
        )
{
        
	NgNclState	ncl = (NgNclState)_NhlGetLayer(nclstate);
	NgNclStatePart	*ns;
        int		endlen,addlen,buflen;
        int		i;
        
            /* Note that string values are expected to be already
               enclosed in escaped quotes */
        if (! ncl) {
                NHLPERROR((NhlFATAL,NhlEUNKNOWN,"Invalid nclstate id"));
                return NhlFATAL;
        }
 
        ns = &ncl->nclstate;

        if (block_id != ns->block_id) {
                NHLPERROR((NhlFATAL,NhlEUNKNOWN,"Ncl block sequence error"));
                return NhlFATAL;
        }
        
        switch (ns->block_type) {
            case _NgCREATE:
                    endlen = strlen("end create\n");
                    break;
            case _NgSETVAL:
                    endlen = strlen("end setvalues\n");
                    break;
            case _NgGETVAL:
                    NHLPERROR((NhlFATAL,NhlEUNKNOWN,
                               "Invalid type for NgNclVisBlockAddCSList"));
                    return NhlFATAL;
        }
        
        for (i = 0; i < res_count; i++) {
                addlen = endlen + 8 + strlen(res_names[i]) + strlen(values[i]);
                buflen = strlen(ns->buffer);
                if (buflen + addlen > ns->bufsize) {
                        ns->buffer = NhlRealloc
                                (ns->buffer,MAX(512,buflen+addlen));
                        NHLPERROR((NhlFATAL,ENOMEM,NULL));
                        return NhlFATAL;
                }
                if (quote && quote[i])
                        sprintf(&ns->buffer[buflen],"\"%s\" : \"%s\"\n",
                                res_names[i],values[i]);
                else
                        sprintf(&ns->buffer[buflen],"\"%s\" : %s\n",
                                res_names[i],values[i]);
        }
        return NhlNOERROR;
}

extern NhlErrorTypes
NgNclCopyShapedVar
(
        int		nclstate,
        NhlString       to_varname,
	NrmQuark	qfile,
	NrmQuark	qvar,
	int		ndims,
	long		*start,
	long		*finish,
	long		*stride
)

{
	NgNclState	ncl = (NgNclState)_NhlGetLayer(nclstate);
	NgNclStatePart	*ns;
	int		i;

        if (! ncl) {
                NHLPERROR((NhlFATAL,NhlEUNKNOWN,"Invalid nclstate id"));
                return NhlFATAL;
        }
        ns = &ncl->nclstate;
        if (! ns->buffer) {
                ns->buffer = NhlMalloc(512);
                if (!ns->buffer) {
                        NHLPERROR((NhlFATAL,ENOMEM,NULL));
                        return NhlFATAL;
                }
                ns->bufsize = 512;
        }

        if (qfile)
                sprintf(ns->buffer,"%s = %s->%s(",
                        NgNclGetSymName(nclstate,to_varname,False),
                        NrmQuarkToString(qfile),
                        NrmQuarkToString(qvar));
        else
                sprintf(ns->buffer,"%s = %s(",
                        NgNclGetSymName(nclstate,to_varname,False),
                        NrmQuarkToString(qvar));
        for (i = 0; i < ndims; i++) 
		sprintf(&ns->buffer[strlen(ns->buffer)],"%ld:%ld:%ld,",
			start[i],finish[i],stride[i]);
        
        	/* backup 1 to remove last comma */
	sprintf(&ns->buffer[strlen(ns->buffer)-1],")\n");

	(void)NgNclSubmitBlock(nclstate,ns->buffer);

        return NhlNOERROR;
}
