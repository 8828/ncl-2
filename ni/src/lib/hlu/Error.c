/*
 *      $Id: Error.c,v 1.15 1994-11-23 22:41:39 ethan Exp $
 */
/************************************************************************
*									*
*			     Copyright (C)  1992			*
*	     University Corporation for Atmospheric Research		*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		Error.c
 *
 *	Author:		Jeff W. Boote
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Wed Sep 2 14:01:23 MDT 1992
 *
 *	Description:	This file contains the error message functions
 *			used by the hlu's.  It has functionality simular
 *			to the ESprintf procedures, but an error object
 *			is formed to hide the implimentation from the
 *			user as well as to allow the user to configure
 *			the error system in the same way they configure
 *			plots.  (ie. by setting resources)
 *			It also incompases some added functionality not
 *			addressed by ESprintf - by allowing the user to send
 *			error messages to a specific file descriptor. And by
 *			default the error messages will be sent to stderr.
 *			Many of the functions in this file are based on
 *			the ESprintf functions written by John Clyne.
 */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ncarg/hlu/ErrorP.h>
#include <ncarg/hlu/VarArg.h>
#include <ncarg/hlu/ConvertersP.h>

#define	MAXERRMSGLEN	10240
#define	TABLELISTINC	10
#define	ERRLISTINC	32

static char		def_file[] = "stderr";
static NhlErrorLayer	errorLayer = NULL;
#define	DEF_FILE	def_file
#define	DEF_UNIT	(74)

/************************************************************************
*									*
*	Error Class definition's and declarations			*
*									*
************************************************************************/

/* Resources */
#define Oset(field)	NhlOffset(NhlErrorLayerRec,error.field)
static NhlResource resources[] = {

/* Begin-documented-resources */

	{NhlNerrBuffer,NhlCerrBuffer,NhlTBoolean,sizeof(NhlBoolean),
		Oset(buffer_errors),NhlTImmediate,False,0,NULL},
	{NhlNerrLevel,NhlCerrLevel,NhlTErrorTypes,sizeof(NhlErrorTypes),
		Oset(error_level),NhlTImmediate,(NhlPointer)NhlWARNING,0,NULL},
	{NhlNerrPrint,NhlCerrPrint,NhlTBoolean,sizeof(NhlBoolean),
		Oset(print_errors),NhlTImmediate,(NhlPointer)True,0,NULL},
	{NhlNerrFileName,NhlCerrFileName,NhlTString,sizeof(NhlString),
		Oset(error_file),NhlTImmediate,(NhlPointer)DEF_FILE,0,
							(_NhlFreeFunc)NhlFree},
/* End-documented-resources */

	{_NhlNerrMode,_NhlCerrMode,NhlTInteger,sizeof(_NhlC_OR_F),
		Oset(error_mode),NhlTImmediate,(NhlPointer)_NhlNONE,0,NULL}
};
#undef Oset

#define Oset(field)	NhlOffset(_NhlErrorLayerCRec,cerror.field)
static NhlResource cresources[] = {

/* Begin-documented-resources */

	{NhlNerrFilePtr,NhlCerrFilePtr,NhlTPointer,sizeof(NhlPointer),
		Oset(fp),NhlTImmediate,(NhlPointer)NULL,0,NULL}

/* End-documented-resources */

};
#undef Oset

#define Oset(field)	NhlOffset(_NhlErrorLayerFRec,ferror.field)
static NhlResource fresources[] = {

/* Begin-documented-resources */

	{NhlNerrUnitNumber,NhlCerrUnitNumber,NhlTInteger,sizeof(int),
		Oset(eunit),NhlTImmediate,(NhlPointer)-1,0,NULL}

/* End-documented-resources */

};
#undef Oset

/* Methode declarations	*/

static NhlErrorTypes ErrorClassPartInitialize(
#if	NhlNeedProto
	NhlLayerClass	lc
#endif
);

static NhlErrorTypes ErrorClassInitialize(
#if	NhlNeedProto
	void
#endif
);

static NhlErrorTypes ErrorInitialize(
#if	NhlNeedProto
	NhlLayerClass	lc,	/* class	*/
	NhlLayer	req,	/* requested	*/
	NhlLayer	new,	/* new		*/
	_NhlArgList	args,	/* args		*/
	int		nargs	/* nargs	*/
#endif
);

static NhlErrorTypes ErrorSetValues(
#if	NhlNeedProto
	NhlLayer	old,		/* old		*/
	NhlLayer	req,		/* requested	*/
	NhlLayer	new,		/* new		*/
	_NhlArgList	args,		/* args to set	*/
	int		nargs		/* nargs	*/
#endif
);

static NhlErrorTypes ErrorDestroy(
#if	NhlNeedProto
	NhlLayer	l	/* layer to destroy	*/
#endif
);

static NhlErrorTypes ErrorGetValues(
#if	NhlNeedProto
	NhlLayer	/* l */,
	_NhlArgList 	/* args */,
	int		/* nargs */
#endif
);

/* Class definition	*/

NhlErrorLayerClassRec NhlerrorLayerClassRec = {
	/* BaseClassPart */
	{
/* class_name			*/	"errorLayerClass",
/* nrm_class			*/	NrmNULLQUARK,
/* layer_size			*/	sizeof(NhlErrorLayerRec),
/* class_inited			*/	False,
/* superclass			*/	(NhlLayerClass)&NhlbaseLayerClassRec,

/* layer_resources		*/	resources,
/* num_resources		*/	NhlNumber(resources),
/* all_resources		*/	NULL,

/* class_part_initialize	*/	ErrorClassPartInitialize,
/* class_initialize		*/	ErrorClassInitialize,
/* layer_initialize		*/	ErrorInitialize,
/* layer_set_values		*/	ErrorSetValues,
/* layer_set_values_hook	*/	NULL,
/* layer_get_values		*/	ErrorGetValues,
/* layer_reparent		*/	NULL,
/* layer_destroy		*/	ErrorDestroy,
/* child_resources		*/	NULL,
/* layer_draw			*/	NULL,
/* layer_pre_draw		*/	NULL,
/* layer_draw_segonly		*/	NULL,
/* layer_post_draw		*/	NULL,
/* layer_clear			*/	NULL
	},
	/* ErrorClassPart */
	{
/* num_error_instances		*/	0
	}
};

static _NhlErrorLayerCClassRec _NhlerrorLayerCClassRec = {
	/* BaseClassPart */
	{
/* class_name			*/	"errorCLayerClass",
/* nrm_class			*/	NrmNULLQUARK,
/* layer_size			*/	sizeof(_NhlErrorLayerCRec),
/* class_inited			*/	False,
/* superclass			*/	(NhlLayerClass)&NhlobjLayerClassRec,

/* layer_resources		*/	cresources,
/* num_resources		*/	NhlNumber(cresources),
/* all_resources		*/	NULL,

/* class_part_initialize	*/	NULL,
/* class_initialize		*/	NULL,
/* layer_initialize		*/	NULL,
/* layer_set_values		*/	NULL,
/* layer_set_values_hook	*/	NULL,
/* layer_get_values		*/	NULL,
/* layer_reparent		*/	NULL,
/* layer_destroy		*/	NULL
	},
	/* ErrorClassCPart */
	{
/* foo				*/	0
	}
};

static _NhlErrorLayerFClassRec _NhlerrorLayerFClassRec = {
	/* BaseClassPart */
	{
/* class_name			*/	"errorFLayerClass",
/* nrm_class			*/	NrmNULLQUARK,
/* layer_size			*/	sizeof(_NhlErrorLayerFRec),
/* class_inited			*/	False,
/* superclass			*/	(NhlLayerClass)&NhlobjLayerClassRec,

/* layer_resources		*/	fresources,
/* num_resources		*/	NhlNumber(fresources),
/* all_resources		*/	NULL,

/* class_part_initialize	*/	NULL,
/* class_initialize		*/	NULL,
/* layer_initialize		*/	NULL,
/* layer_set_values		*/	NULL,
/* layer_set_values_hook	*/	NULL,
/* layer_get_values		*/	NULL,
/* layer_reparent		*/	NULL,
/* layer_destroy		*/	NULL
	},
	/* ErrorFClassPart */
	{
/* foo				*/	0
	}
};

NhlLayerClass NhlerrorLayerClass = (NhlLayerClass)&NhlerrorLayerClassRec;
static NhlLayerClass _NhlerrorLayerCClass =
					(NhlLayerClass)&_NhlerrorLayerCClassRec;
static NhlLayerClass _NhlerrorLayerFClass =
					(NhlLayerClass)&_NhlerrorLayerFClassRec;

/************************************************************************
* New type converters - needed for new type's defined for this class	*
* Added to the converter table by ErrorClassInitialize			*
************************************************************************/

/************************************************************************
*									*
*	Error Class Methode definitions					*
*									*
************************************************************************/

/*
 * Function:	ErrorClassPartInitialize
 *
 * Description:	Class part init
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
ErrorClassPartInitialize
#if	NhlNeedProto
(
	NhlLayerClass	lc	/* NhlLayerClass to init */
)
#else
(lc)
	NhlLayerClass	lc;	/* NhlLayerClass to init */
#endif
{
	NhlErrorTypes		ret,lret;

	lret = _NhlRegisterChildClass(lc,_NhlerrorLayerCClass,True,False,NULL);
	ret = _NhlRegisterChildClass(lc,_NhlerrorLayerFClass,True,False,NULL);

	return MIN(lret,ret);
}

/*
 * Function:	ErrorClassInitialize
 *
 * Description:	This function initializes the Error Class structure
 *
 * In Args:	none
 *
 * Out Args:	none
 *
 * Scope:	static
 * Returns:	NhlErrorTypes
 * Side Effect:	
 */
static NhlErrorTypes
ErrorClassInitialize
#if	NhlNeedProto
(
	void
)
#else
()
#endif
{
	_NhlEnumVals	errtypes[] = {
				{NhlFATAL,	"fatal"},
				{NhlWARNING,	"warning"},
				{NhlINFO,	"info"},
				{NhlNOERROR,	"noerror"}
				};

	/*
	 *	Install converters to the NhlErrorTypes enum.
	 */
	_NhlRegisterEnumType(NhlTErrorTypes,errtypes,NhlNumber(errtypes));

	return NhlNOERROR;
}

/*
 * Function:	ErrorInitialize
 *
 * Description:	This function initializes an instance of an Error class
 *
 * In Args:	
 *	NhlLayerClass	lc,	class
 *	NhlLayer	req,	requested
 *	NhlLayer	new,	new
 *	_NhlArgList	args,	args
 *	int		nargs	nargs
 *
 * Out Args:	
 *
 * Scope:	static
 * Returns:	NhlErrorTypes
 * Side Effect:	
 */
/*ARGSUSED*/
static NhlErrorTypes
ErrorInitialize
#if	NhlNeedProto
(
	NhlLayerClass	lc,	/* class	*/
	NhlLayer	req,	/* requested	*/
	NhlLayer	new,	/* new		*/
	_NhlArgList	args,	/* args		*/
	int		nargs	/* nargs	*/
)
#else
(lc,req,new,args,nargs)
	NhlLayerClass	lc;	/* class	*/
	NhlLayer	req;	/* requested	*/
	NhlLayer	new;	/* new		*/
	_NhlArgList	args;	/* args		*/
	int		nargs;	/* nargs	*/
#endif
{
	extern int	sys_nerr;
	extern char	*sys_errlist[];
	NhlErrorLayerClass	elc = (NhlErrorLayerClass)lc;
	NhlErrorLayer		enew = (NhlErrorLayer)new;
	NhlLayerClass		childclass;
	Const char		*tfname = NULL;
	NhlErrorTypes		ret;
	char			*fname = "ErrorInitialize";
	int			foo=0;

	/*
	 * This is just here to force the gerhnd function to load from the
	 * hlu library - It needs to be referenced from this library or
	 * that won't happen.
	 */
	_NHLCALLF(nhlfloadgerhnd,NHLFLOADGERHND)(&foo);

	if(elc->error_class.num_error_instances > 0){
		NHLPERROR((NhlFATAL,NhlEUNKNOWN,
		"%s:Only one instance of ErrorClass is supported",fname));
		return NhlFATAL;
	}

	if(enew->error.error_mode == _NhlCLIB)
		childclass = _NhlerrorLayerCClass;
	else if(enew->error.error_mode == _NhlFLIB)
		childclass = _NhlerrorLayerFClass;
	else{
		NHLPERROR((NhlFATAL,NhlEUNKNOWN,"%s:Invalid Error Mode",fname));
		return NhlFATAL;
	}

	ret = _NhlVACreateChild(&enew->error.child,NULL,childclass,
							(NhlLayer)enew,NULL);

	if (ret < NhlWARNING)
		return ret;

	/*
	 * Put libncarg/libncarg_gks in recovery mode.
	 */
	c_entsr(&foo,1);

	tfname = enew->error.error_file;

	if(enew->error.error_mode == _NhlCLIB){
		_NhlErrorLayerC	cchild = (_NhlErrorLayerC)
						_NhlGetLayer(enew->error.child);
		cchild->cerror.my_fp = False;
		if(strcmp(tfname,"stderr") == 0)
			cchild->cerror.fp = stderr;
		else if(strcmp(tfname,"stdout") == 0)
			cchild->cerror.fp = stdout;
		else{
			tfname = _NGResolvePath(enew->error.error_file);
			cchild->cerror.fp = fopen(tfname,"w");
			if(cchild->cerror.fp == NULL){
				NHLPERROR((NhlWARNING,errno,
			"%s:Unable to open error file:%s - Using stderr",fname,
								tfname));
				ret = MIN(ret,NhlWARNING);
				cchild->cerror.fp = stderr;
				tfname = "stderr";
			}
			else
				cchild->cerror.my_fp = True;
		}
		enew->error.private_fp = cchild->cerror.fp;
	}
	else if(enew->error.error_mode == _NhlFLIB){
		_NhlErrorLayerF	fchild = (_NhlErrorLayerF)
						_NhlGetLayer(enew->error.child);

		fchild->ferror.my_eunit = False;
		if(fchild->ferror.eunit != -1){
			/* Unit Number specified */
			if(tfname != DEF_FILE){
				/* File is specified so open unit with file */
				int	conn = 0;
				int	ierr = 0;
				_NHLCALLF(nhl_finqunit,NHLF_INQUNIT)
					(&fchild->ferror.eunit,&conn,&ierr);
				if(ierr){
					NhlPError(NhlWARNING,NhlEUNKNOWN,
				"%s:Fortran I/O Error #%n:Using default I/O",
								fname,ierr);
					conn = 4;
					fchild->ferror.eunit =
						_NHLCALLF(i1mach,I1MACH)(&conn);
					tfname = "stderr";
				}
				else if(conn){
					NhlPError(NhlWARNING,NhlEUNKNOWN,
			"%s:Ignoring %s=%s :Unit number %d already open",fname,
							NhlNerrFileName,tfname,
							fchild->ferror.eunit);
					ret = MIN(ret,NhlWARNING);
					tfname = DEF_FILE;
				}
				else{
					_NhlFString	ffname;
					int		ffname_len;

					tfname =
					_NGResolvePath(enew->error.error_file);

					if(tfname){

						ffname_len = strlen(tfname);
						ffname = (_NhlFString)
							_NhlCptrToFptr(tfname);
						_NHLCALLF(nhl_fopnunit,
								NHLF_OPNUNIT)
							(&fchild->ferror.eunit,
							ffname,&ffname_len,
									&ierr);
					}

					if(!tfname || ierr){

					NhlPError(NhlWARNING,NhlEUNKNOWN,
	"%s:Unable to open %s with Unit %d:reverting to defaults",fname,tfname,
							fchild->ferror.eunit);
						conn = 4;
						fchild->ferror.eunit =
						_NHLCALLF(i1mach,I1MACH)(&conn);
						tfname = "stderr";
					}
				}
			}
		}
		else{
			int tint = 4;

			/* Unit number not specified */
			if(tfname == DEF_FILE){
				/* file not specified - use stderr */
				tfname = DEF_FILE;
				fchild->ferror.eunit =
						_NHLCALLF(i1mach,I1MACH)(&tint);
			}
			else{
				if(strcmp(tfname,"stderr") == 0)
					fchild->ferror.eunit =
						_NHLCALLF(i1mach,I1MACH)(&tint);
				else if(strcmp(tfname,"stdout") == 0){
					tint = 2;
					fchild->ferror.eunit =
						_NHLCALLF(i1mach,I1MACH)(&tint);
				}
				else{
					_NhlFString	ffname;
					int		ffname_len;
					int		ierr;

					fchild->ferror.eunit = DEF_UNIT;

					tfname =
					_NGResolvePath(enew->error.error_file);

					if(tfname){
						ffname_len = strlen(tfname);
						ffname = (_NhlFString)
							_NhlCptrToFptr(tfname);
						_NHLCALLF(nhl_fopnunit,
								NHLF_OPNUNIT)
							(&fchild->ferror.eunit,
							ffname,&ffname_len,
									&ierr);
					}

					if(!tfname || ierr){
						int tint = 4;

					NhlPError(NhlWARNING,NhlEUNKNOWN,
	"ErrorInit:Unable to open %s with Unit %d:reverting to defaults",tfname,
							fchild->ferror.eunit);
						tfname = DEF_FILE;
						fchild->ferror.eunit =
						_NHLCALLF(i1mach,I1MACH)(&tint);
					}
					else
						fchild->ferror.my_eunit = True;
				}
			}
		}
		enew->error.private_eunit = fchild->ferror.eunit;
	}

	if(!tfname)
		tfname = DEF_FILE;

	enew->error.error_file = (char *)NhlMalloc(strlen(tfname) + 1);
	strcpy(enew->error.error_file,tfname);

	enew->error.num_emsgs = 0;
	enew->error.emsgs = NULL;
	enew->error.len_emsgs = 0;

	enew->error.num_etables = 0;
	enew->error.etables = NULL;
	enew->error.len_etables = 0;

	elc->error_class.num_error_instances = 1;

	errorLayer = enew;

	ret = NhlErrAddTable(0,sys_nerr,(Const char **)sys_errlist);
	if (ret != NhlNOERROR){
		NHLPERROR((ret,NhlEUNKNOWN,"Error loading System Error Table"));
	}

	return(ret);
}

/*
 * Function:	ErrorSetValues
 *
 * Description:	This is the setvalues methode for the errorclass hlu.
 *
 * In Args:	
 *		NhlLayer		old,		old
 *		NhlLayer		req,		requested
 *		_NhlArgList	args,		args to set
 *		int		nargs		nargs
 *
 * Out Args:	
 *		NhlLayer		new,		new
 *
 * Scope:	static
 * Returns:	NhlErrorTypes
 * Side Effect:	
 */
/*ARGSUSED*/
static NhlErrorTypes
ErrorSetValues
#if	NhlNeedProto
(
	NhlLayer		old,		/* old		*/
	NhlLayer		req,		/* requested	*/
	NhlLayer		new,		/* new		*/
	_NhlArgList	args,		/* args to set	*/
	int		nargs		/* nargs	*/
)
#else
(old,req,new,args,nargs)
	NhlLayer		old;		/* old		*/
	NhlLayer		req;		/* requested	*/
	NhlLayer		new;		/* new		*/
	_NhlArgList	args;		/* args to set	*/
	int		nargs;		/* nargs	*/
#endif
{
	NhlErrorTypes	ret = NhlNOERROR;
	NhlErrorLayer	eold = (NhlErrorLayer)old;
	NhlErrorLayer	enew = (NhlErrorLayer)new;
	char		*fname = "ErrorSetValues";
	Const char	*tfname = NULL;

	/*
	 * Silently ignore changes to mode. It shouldn't be possible, but
	 * just in case.
	 */
	enew->error.error_mode = eold->error.error_mode;

	if(eold->error.error_file != enew->error.error_file){

		tfname = enew->error.error_file;

		if(enew->error.error_mode == _NhlCLIB){
			_NhlErrorLayerC	cchild = (_NhlErrorLayerC)
						_NhlGetLayer(enew->error.child);

			if(cchild->cerror.my_fp){
				if(fclose(eold->error.private_fp) != 0){
					NhlPError(NhlWARNING,NhlEUNKNOWN,
						"Error closing old error file");
					ret = MIN(ret,NhlWARNING);
				}
			}

			cchild->cerror.my_fp = False;
			if(enew->error.private_fp != cchild->cerror.fp){
				if(enew->error.error_file != NULL){
					NhlPError(NhlWARNING,NhlEUNKNOWN,
						"%s:%s specified - ignoring %s",
							fname,NhlNerrFilePtr,
							NhlNerrFileName);
				}
				tfname = DEF_FILE;
				enew->error.private_fp = cchild->cerror.fp;
			}
			else{
				FILE	*tfptr = NULL;

				if(tfname == NULL){
					NhlPError(NhlWARNING,NhlEUNKNOWN,
				"Unable to open file NULL, Using stderr");
					ret = MIN(ret,NhlWARNING);
					tfname = "stderr";
				}

				if(strcmp(tfname,"stderr") == 0)
					tfptr = stderr;
				else if(strcmp(tfname,"stdout") == 0)
					tfptr = stdout;
				else{
					tfname =
					_NGResolvePath(enew->error.error_file);
					if(tfname)
						tfptr = fopen(tfname,"w");
					if(!tfname || !tfptr){
					NHLPERROR((NhlWARNING,NhlEUNKNOWN,
					"%s:Unable to open %s,Using stderr",
						fname,(tfname)?tfname:"nil"));
						ret = MIN(ret,NhlWARNING);
						tfptr = stderr;
						tfname = "stderr";
					}
					else
						cchild->cerror.my_fp = True;
				}
				enew->error.private_fp = cchild->cerror.fp =
									tfptr;
			}
		}
		else if(enew->error.error_mode == _NhlFLIB){
			_NhlErrorLayerF	fchild = (_NhlErrorLayerF)
						_NhlGetLayer(enew->error.child);
			int	ierr = 0;
			int	conn = 0;

			if(fchild->ferror.my_eunit)
				_NHLCALLF(nhl_fclsunit,NHLF_CLSUNIT)
					(&eold->error.private_eunit,&ierr);
			else
				_NHLCALLF(nhl_finqunit,NHLF_INQUNIT)
					(&fchild->ferror.eunit,&conn,&ierr);

			fchild->ferror.my_eunit = False;

			if(ierr){
				NhlPError(NhlWARNING,NhlEUNKNOWN,
				"%s:Fortran I/O error #%n - Using defaults",
								fname,ierr);
				tfname = "stderr";
				conn = 4;
				fchild->ferror.eunit = _NHLCALLF(i1mach,I1MACH)
									(&conn);
			}
			else if(conn){
				NhlPError(NhlWARNING,NhlEUNKNOWN,
			"%s:Ignoring %s=%s :Unit number %d already open",fname,
							NhlNerrFileName,tfname,
							fchild->ferror.eunit);
				ret = MIN(ret,NhlWARNING);
				tfname = DEF_FILE;
			}
			else{
				_NhlFString	ffname;
				int		ffname_len;

				tfname =_NGResolvePath(enew->error.error_file);

				if(tfname){
					ffname_len = strlen(tfname);
					ffname = (_NhlFString)
							_NhlCptrToFptr(tfname);
					_NHLCALLF(nhl_fopnunit,NHLF_OPNUNIT)
						(&fchild->ferror.eunit,ffname,
							&ffname_len,&ierr);
				}

				if(!tfname || ierr){
					NhlPError(NhlWARNING,NhlEUNKNOWN,
		"%s:Unable to open %s with Unit %d:reverting to defaults",fname,
							(tfname)?tfname:"nil",
							fchild->ferror.eunit);
					conn = 4;
					fchild->ferror.eunit =
						_NHLCALLF(i1mach,I1MACH)(&conn);
					tfname = "stderr";
				}
			}
			enew->error.private_eunit = fchild->ferror.eunit;
		}
		else{
			NhlPError(NhlFATAL,NhlEUNKNOWN,
					"%s:Unknown error_mode???",fname);
			return NhlFATAL;
		}

		(void)NhlFree(eold->error.error_file);
		if(!tfname)
			tfname = DEF_FILE;
		enew->error.error_file = (char *)NhlMalloc(strlen(tfname) + 1);
		if(!enew->error.error_file){
			NHLPERROR((NhlFATAL,ENOMEM,NULL));
			return NhlFATAL;
		}
		strcpy(enew->error.error_file,tfname);
	}
	else{

		if(enew->error.error_mode == _NhlCLIB){
			_NhlErrorLayerC	cchild = (_NhlErrorLayerC)
						_NhlGetLayer(enew->error.child);
			if(enew->error.private_fp != cchild->cerror.fp){


				if(!cchild->cerror.fp){
					NhlPError(NhlWARNING,NhlEUNKNOWN,
					"%s:%s cannot be NULL - Ignoring",fname,
					NhlNerrFilePtr);
					cchild->cerror.fp =
							enew->error.private_fp;
				}
				else
					enew->error.private_fp =
							cchild->cerror.fp;
				if(cchild->cerror.my_fp){
					if(fclose(eold->error.private_fp) != 0){
					NhlPError(NhlWARNING,NhlEUNKNOWN,
						"Error closing old error file");
						ret = MIN(ret,NhlWARNING);
					}
				}
				cchild->cerror.my_fp = False;

				if(cchild->cerror.fp == stderr)
					tfname = "stderr";
				else if(cchild->cerror.fp == stdout)
					tfname = "stdout";
				else
					tfname = DEF_FILE;
			}
		}
		else if(enew->error.error_mode == _NhlFLIB){
			_NhlErrorLayerF	fchild = (_NhlErrorLayerF)
						_NhlGetLayer(enew->error.child);
			if(enew->error.private_eunit != fchild->ferror.eunit){
				int conn;
				int ierr = 0;

				enew->error.private_eunit =fchild->ferror.eunit;

				if(fchild->ferror.my_eunit)
					_NHLCALLF(nhl_fclsunit,NHLF_CLSUNIT)
					(&eold->error.private_eunit,&ierr);

				if(ierr){
					NhlPError(NhlWARNING,NhlEUNKNOWN,
						"%s:Error Closing Unit #%n",
						fname,
						eold->error.private_eunit);
				}

				fchild->ferror.my_eunit = False;

				conn = 4;
				if(fchild->ferror.eunit ==
						_NHLCALLF(i1mach,I1MACH)(&conn))
					tfname = "stderr";
				else{
					conn = 2;
					if(fchild->ferror.eunit ==
						_NHLCALLF(i1mach,I1MACH)(&conn))

						tfname = "stdout";
					else
						tfname = DEF_FILE;
				}
			}
		}
		else{
			NhlPError(NhlFATAL,NhlEUNKNOWN,
					"%s:Unknown Error_mode???",fname);
			return NhlFATAL;
		}

		(void)NhlFree(eold->error.error_file);
		if(!tfname)
			tfname = DEF_FILE;
		enew->error.error_file = (char *)NhlMalloc(strlen(tfname) + 1);
		if(!enew->error.error_file){
			NHLPERROR((NhlFATAL,ENOMEM,NULL));
			return NhlFATAL;
		}
		strcpy(enew->error.error_file,tfname);
	}

	return ret;
}

/*
 * Function:	ErrorDestroy
 *
 * Description:	This function free's any memory that has been alocated
 *		on behalf of the ErrorClass instance.
 *
 * In Args:	NhlLayer	l	The layer to destroy
 *
 * Out Args:	
 *
 * Scope:	static
 * Returns:	NhlErrorTypes
 * Side Effect:	
 */
static NhlErrorTypes
ErrorDestroy
#if	NhlNeedProto
(
	NhlLayer l		/* layer to ready for destruction	*/
)
#else
(l)
	NhlLayer l;	/* layer to ready for destruction	*/
#endif
{
	NhlErrorTypes	ret = NhlNOERROR;
	NhlErrorLayer	el = (NhlErrorLayer)l;
	int		i;
	char		*fname = "ErrorDestroy";

	if(el->error.error_mode == _NhlCLIB){
		_NhlErrorLayerC	cchild = (_NhlErrorLayerC)
						_NhlGetLayer(el->error.child);
		if(cchild->cerror.my_fp){
			if(fclose(cchild->cerror.fp) != 0){
				NHLPERROR((NhlWARNING,errno,
						"%s:Error closing error file"));
				ret = MIN(ret,NhlWARNING);
			}
		}
	}
	else if(el->error.error_mode == _NhlFLIB){
		_NhlErrorLayerF	fchild = (_NhlErrorLayerF)
						_NhlGetLayer(el->error.child);
		int ierr = 0;

		if(fchild->ferror.my_eunit){
			_NHLCALLF(nhl_fclsunit,NHLF_CLSUNIT)
						(&fchild->ferror.eunit,&ierr);
			if(ierr){
				NHLPERROR((NhlWARNING,NhlEUNKNOWN,
					"%s:Error closing Unit #%n,IOSTAT=%n",
					fname,fchild->ferror.eunit,ierr));
				ret = MIN(ret,NhlWARNING);
			}
		}
	}

	(void)NhlFree(el->error.error_file);
	for(i=0;i < el->error.num_emsgs;i++){
		(void)NhlFree(el->error.emsgs[i].msg);
		(void)NhlFree(el->error.emsgs[i].fname);
	}
	(void)NhlFree(el->error.emsgs);
	(void)NhlFree(el->error.etables);

	errorLayer = NULL;

	return ret;
}

/************************************************************************
*									*
*	Private API for Error handling					*
*									*
************************************************************************/

/*
 * Function:	RetrieveSysError
 *
 * Description:	This function retrieves a pointer to the error message
 *		indicated by the specified error number. Or NULL if
 *		that error number doesn't exist.
 *
 * In Args:	
 *		int	errnum	error number to retrieve message for
 *
 * Out Args:	
 *
 * Scope:	static
 * Returns:	Const char *
 * Side Effect:	
 */
static Const char *
RetrieveSysError
#if	NhlNeedProto
(
	int	errnum	/* error number to retrieve message for	*/
)
#else
(errnum)
	int	errnum;	/* error number to retrieve message for	*/
#endif
{
	int i, error;

	if(errorLayer == NULL)
		return NULL;

	for(i=0;i < errorLayer->error.num_etables;i++){
		error = errnum - errorLayer->error.etables[i].start;
		if((error >= 0) && (error < errorLayer->error.etables[i].len)){
			return errorLayer->error.etables[i].errs[error];
		}
	}

	return NULL;
}

/*
 * Function:	BufferMsg
 *
 * Description:	This function copies a static message structure into the
 *		error instance's internal data.
 *
 * In Args:	
 *		Const NhlErrMsg	*msg	message to buffer
 *
 * Out Args:	
 *
 * Scope:	static
 * Returns:	void
 * Side Effect:	
 */
static void
BufferMsg
#if	NhlNeedProto
(
	Const NhlErrMsg	*msg	/* message to buffer	*/
)
#else
(msg)
	Const NhlErrMsg	*msg;	/* message to buffer	*/
#endif
{
	/* increase size of errmsg table if neccessary */
	if(errorLayer->error.len_emsgs < errorLayer->error.num_emsgs + 1){
		errorLayer->error.emsgs = (NhlErrMsgList)
				NhlRealloc(errorLayer->error.emsgs,
				((errorLayer->error.len_etables + ERRLISTINC) *
							sizeof(NhlErrMsg)));
		if(errorLayer->error.emsgs == NULL){
			errorLayer->error.num_emsgs = 0;
			errorLayer->error.len_emsgs = 0;
			errorLayer->error.buffer_errors = False;
			errorLayer->error.print_errors = True;
			NHLPERROR((NhlWARNING,ENOMEM,
						"Unable to buffer e-msgs"));
		}
		else
			errorLayer->error.len_emsgs += ERRLISTINC;
	}

	/*
	 * copy errmsg into buffer
	 */
	errorLayer->error.emsgs[errorLayer->error.num_emsgs] = *msg;
	errorLayer->error.emsgs[errorLayer->error.num_emsgs].msg =
				(char *)NhlMalloc(strlen(msg->msg) + 1);
	strcpy(errorLayer->error.emsgs[errorLayer->error.num_emsgs].msg,
								msg->msg);
	errorLayer->error.num_emsgs++;
}

/*
 * Function:	PrintMessage
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
static Const char *
PrintMessage
#if	NhlNeedProto
(
	Const NhlErrMsg	*msg		/* message to print		*/
)
#else
(msg)
	Const NhlErrMsg	*msg;		/* message to print		*/
#endif
{
	if(errorLayer->error.error_mode == _NhlFLIB){
		static char	tbuf[MAXERRMSGLEN];
		Const char	*message;
		_NhlFString	fmessage;
		int		fmessage_len;

		message = NhlErrSPrintMsg(tbuf,msg);
		fmessage_len = strlen(message);
		fmessage = (_NhlFString)_NhlCptrToFptr(message);
		_NHLCALLF(nhl_fprnmes,NHLF_PRNMES)
				(&errorLayer->error.private_eunit,fmessage,
								&fmessage_len);

		return tbuf;
	}
	else if(errorLayer->error.error_mode == _NhlCLIB)
		return NhlErrFPrintMsg(errorLayer->error.private_fp,msg);
	else
		return NhlErrFPrintMsg(stderr,msg);
}
	
/*
 * Function:	AddErrMsg
 *
 * Description:	This function uses the internal state of the error instance
 *		to determine which error messages to print out and which
 *		ones to buffer etc...
 *
 * In Args:	
 *		Const NhlErrMsg	*msg	message to add
 *
 * Out Args:	
 *
 * Scope:	static
 * Returns:	Const char *
 * Side Effect:	
 */
static Const char *
AddErrMsg
#if	NhlNeedProto
(
	Const NhlErrMsg	*msg	/* message to add	*/
)
#else
(msg)
	Const NhlErrMsg	*msg;	/* message to add	*/
#endif
{
	static char buffer[MAXERRMSGLEN];

	/* if error instance not init'ed - print to sterr */
	if(errorLayer == NULL)
		return NhlErrFPrintMsg(stderr,msg);

	/* if msg severity > level requested return NULL */
	if(msg->severity > errorLayer->error.error_level){
		return NULL;
	}

	/* buffer message */
	if(errorLayer->error.buffer_errors){
		BufferMsg(msg);
	}

	/* print out message */
	if(errorLayer->error.print_errors)
		return PrintMessage(msg);
	else
		return NhlErrSPrintMsg(buffer,msg);
}

/************************************************************************
*									*
*	Public API for Error handling					*
*									*
************************************************************************/
/*
 * These var's are used to support the VarArg macro hack.
 */
static HackUsed = False;
static struct {
	int	line;
	char	fname[_NhlMAXFNAMELEN];
} HackInfo;

/*
 * Function:	_NhlPErrorHack
 *
 * Description:	This function saves line and file information for NhlPerror
 *		to retrieve when it is called.  It is a gross ugly hack
 *		to allow a vararg macro call.
 *
 * In Args:	int	line;		line number
 *		char	*fname;		file name
 *
 * Out Args:	
 *
 * Scope:	Global Private -	It should only be called by the
 *					NHLPERROR macro.
 * Returns:	
 * Side Effect:	
 */
void
_NhlPErrorHack
#if	NhlNeedProto
(
	int		line,	/* line number	*/
	Const char	*fname	/* file name	*/
)
#else
(line,fname)
	int		line;	/* line number	*/
	Const char	*fname;	/* file name	*/
#endif
{
	HackUsed = True;

	HackInfo.line = line;
	strcpy(HackInfo.fname,fname);

	return;
}

/*
 * Function:	printerror
 *
 * Description:	This is the function used to report an error.
 *
 * In Args:	
 *		NhlErrorTypes	severity,	error severity
 *		int		errnum,		errornum in table
 *		char		*errstring,	fmt string
 *
 * Out Args:	
 *
 * Scope:	Global Public
 * Returns:	Const char * - the string it will print or buffer
 * Side Effect:	
 */
static Const char *printerror
#if     NhlNeedProto
(
	NhlErrorTypes	severity,	/* error severity	*/
	int		errnum,		/* errornum in table	*/
	NhlString	errstring	/* fmt string		*/
)
#else
(severity,errnum,errstring)
	NhlErrorTypes	severity;	/* error severity	*/
	int		errnum;		/* errornum in table	*/
	NhlString	errstring;	/* fmt string		*/
#endif
{
	NhlErrMsg	tmp;

	tmp.severity = severity;
	tmp.errorno = errnum;
	tmp.msg = errstring;

	if(errnum == NhlEUNKNOWN)
		tmp.sysmsg = NULL;
	else
		tmp.sysmsg = RetrieveSysError(errnum);

	if(HackUsed){
		tmp.line = HackInfo.line;
		tmp.fname = NhlMalloc(strlen(HackInfo.fname) + 1);
		strcpy(tmp.fname,HackInfo.fname);
		HackUsed = False;
	}
	else{
		tmp.line = -1;
		tmp.fname = NULL;
	}

	return AddErrMsg(&tmp);
}

/*
 * Function:	NhlPError
 *
 * Description:	This is the function used to report an error.  It takes
 *		a fmt string that works identical to printf.
 *
 * In Args:	
 *		NhlErrorTypes	severity,	error severity
 *		int		errnum,		errornum in table
 *		char		*fmt,		fmt string
 *		...				args for fmt string
 *
 * Out Args:	
 *
 * Scope:	Global Public
 * Returns:	Const char * - the string it will print or buffer
 * Side Effect:	
 */
Const char *NhlPError
#if     NhlNeedVarArgProto
(
	NhlErrorTypes	severity,	/* error severity	*/
	int		errnum,		/* errornum in table	*/
	char		*fmt,		/* fmt string		*/
	...				/* args for fmt string	*/
)
#else
(severity,errnum,fmt,va_alist)
	NhlErrorTypes	severity;	/* error severity	*/
	int		errnum;		/* errornum in table	*/
	char		*fmt;		/* fmt string		*/
	va_dcl				/* args for fmt string	*/
#endif
{
	va_list		ap;
	char		tbuf[MAXERRMSGLEN];
	NhlString	errstr;

	if(fmt != NULL){

		VA_START(ap,fmt);
		(void)vsprintf(tbuf, fmt, ap);
		va_end(ap);

		errstr = tbuf;
	}
	else
		errstr = NULL;

	return printerror(severity,errnum,errstr);
}

/*
 * Function:	nhl_fperror
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
void
_NHLCALLF(nhl_fperror,NHL_FPERROR)
#if	NhlNeedProto
(
	_NhlFString	flevel,
	int		*flevel_len,
	int		*errnum,
	_NhlFString	estr,
	int		*estr_len
)
#else
(flevel,flevel_len,errnum,estr,estr_len)
	_NhlFString	flevel;
	int		*flevel_len;
	int		*errnum;
	_NhlFString	estr;
	int		*estr_len;
#endif
{
	char		clevel[_NhlMAXRESNAMLEN];
	NhlErrorTypes	elevel;
	NrmValue	from,to;
	char		emsg[_NhlMAXRESNAMLEN];

	if(!_NhlFstrToCstr(clevel,NhlNumber(clevel),flevel,*flevel_len)){
		NhlPError(NhlFATAL,NhlEUNKNOWN,
		"Unable to report Fortran error message:Invalid error level");
		return;
	}

	from.size = strlen(clevel);
	from.data.strval = clevel;
	to.size = sizeof(NhlErrorTypes);
	to.data.ptrval = &elevel;

	if(NhlConvertData(NhlTString,NhlTErrorTypes,&from,&to) != NhlNOERROR){
		NhlPError(NhlFATAL,NhlEUNKNOWN,
		"Unable to report Fortran error message:Invalid error level");
		return;
	}

	if(!_NhlFstrToCstr(emsg,NhlNumber(emsg),estr,*estr_len)){
		NhlPError(NhlFATAL,NhlEUNKNOWN,
		"Unable to report Fortran error message:Invalid Error String");
		return;
	}

	(void)printerror(elevel,*errnum,emsg);

	return;
}

/*
 * Function:	NhlErrGetID
 *
 * Description:	This function returns the pid associated with the errorLayer
 *		object.  This is needed so the application programmer can
 *		set resources in the error object if they want to.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	Global Public
 * Returns:	The PID associated with errorLayer - Or NhlErrorTypes
 *		if errorLayer is unavailable.
 * Side Effect:	
 */
int
NhlErrGetID
#if	NhlNeedProto
(
	void
)
#else
()
#endif
{
	if(errorLayer != NULL)
		return errorLayer->base.id;

	NHLPERROR((NhlFATAL,NhlEUNKNOWN,"Can't find Error Object"));
	return NhlFATAL;
}

/*
 * Function:	nhl_ferrgetid
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
void
_NHLCALLF(nhl_ferrgetid,NHL_FERRGETID)
#if	NhlNeedProto
(
	int	*id
)
#else
(id)
	int	*id;
#endif
{
	*id = NhlErrGetID();

	return;
}

/*
 * Function:	NhlErrNumMsgs
 *
 * Description:	This function returns the number of error messages that
 *		are currently buffered.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	Global Public
 * Returns:	int number of messages - or NhlErrorTypes on error
 * Side Effect:	
 */
int
NhlErrNumMsgs
#if	NhlNeedProto
(
	void
)
#else
()
#endif
{
	if(errorLayer != NULL)
		return errorLayer->error.num_emsgs;

	NHLPERROR((NhlFATAL,NhlEUNKNOWN,"Can't find Error Object"));
	return NhlFATAL;
}

/*
 * Function:	nhl_ferrnummsgs
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
void
_NHLCALLF(nhl_ferrnummsgs,NHL_FERRNUMMSGS)
#if	NhlNeedProto
(
	int	*nummsgs
)
#else
(nummsgs)
	int	*nummsgs;
#endif
{
	*nummsgs = NhlErrNumMsgs();

	return;
}

/*
 * Function:	NhlErrGetMsg
 *
 * Description:	This function returns a single error message specified
 *		by the number passed to it.
 *
 * In Args:	
 *		int		msgnum	msg num to retrieve
 *
 * Out Args:	
 *		NhlErrMsg	**msg	return msg ptr
 *
 * Scope:	Global Public
 * Returns:	NhlErrorTypes
 * Side Effect:	
 */
NhlErrorTypes
NhlErrGetMsg
#if	NhlNeedProto
(
	int		msgnum,	/* msg num to retrieve	*/
	Const NhlErrMsg	**msg	/* return msg		*/
)
#else
(msgnum,msg)
	int		msgnum;	/* msg num to retrieve	*/
	Const NhlErrMsg	**msg;	/* return msg		*/
#endif
{
	if(errorLayer == NULL){
		NHLPERROR((NhlFATAL,NhlEUNKNOWN,"Can't find Error Object"));
		return NhlFATAL;
	}

	if(((msgnum-1) < errorLayer->error.num_emsgs) && (msgnum > 0)){
		*msg = &errorLayer->error.emsgs[msgnum-1];
		return NhlNOERROR;
	}

	NHLPERROR((NhlFATAL,NhlEUNKNOWN,"message number %d doesn't exist",
								msgnum));
	return NhlFATAL;
}

/*
 * Function:	nhl_ferrgetmsg
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
void
_NHLCALLF(nhl_ferrgetmsg,NHL_FERRGETMSG)
#if	NhlNeedProto
(
	int		*index,
	int		*severity,
	_NhlFString	emesg,
	int		*emesg_len,
	int		*errnum,
	_NhlFString	sys_mesg,
	int		*sys_mesg_len,
	int		*line,
	_NhlFString	fname,
	int		*fname_len,
	int		*err
)
#else
(index,severity,emesg,emesg_len,errnum,sys_mesg,sys_mesg_len,line,fname,
							fname_len,err)
	int		*index;
	int		*severity;
	_NhlFString	emesg;
	int		*emesg_len;
	int		*errnum;
	_NhlFString	sys_mesg;
	int		*sys_mesg_len;
	int		*line;
	_NhlFString	fname;
	int		*fname_len;
	int		*err;
#endif
{
	Const NhlErrMsg	*msg;
	NhlErrorTypes	ret,lret;

	ret = NhlErrGetMsg(*index,&msg);

	if(ret == NhlFATAL){
		NhlPError(NhlFATAL,NhlEUNKNOWN,
			"nhl_ferrgetmsg:Unable to retrieve errmsg #%d",*index);

		*err = ret;

		return;
	}

	*severity = msg->severity;
	lret = _NhlCstrToFstr(emesg,*emesg_len,msg->msg);
	ret = MIN(ret,lret);
	*errnum = msg->errorno;
	lret = _NhlCstrToFstr(sys_mesg,*sys_mesg_len,msg->sysmsg);
	ret = MIN(ret,lret);
	*line = msg->line;
	ret = _NhlCstrToFstr(fname,*fname_len,msg->fname);
	ret = MIN(ret,lret);

	*err = ret;

	return;
}

/*
 * Function:	NhlErrClearMsgs
 *
 * Description:	This function clears the internal error buffer that is managed
 *		by the error object.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	Global Public
 * Returns:	NhlErrorTypes
 * Side Effect:	
 */
NhlErrorTypes
NhlErrClearMsgs
#if	NhlNeedProto
(
	void
)
#else
()
#endif
{
	int	i;

	if(errorLayer == NULL){
		NHLPERROR((NhlFATAL,NhlEUNKNOWN,"Can't find Error Object"));
		return NhlFATAL;
	}

	for(i=0;i < errorLayer->error.num_emsgs;i++){
		(void)NhlFree(errorLayer->error.emsgs[i].msg);
		(void)NhlFree(errorLayer->error.emsgs[i].fname);
	}
	(void)NhlFree(errorLayer->error.emsgs);
	errorLayer->error.emsgs = NULL;
	errorLayer->error.num_emsgs = 0;
	errorLayer->error.len_emsgs = 0;

	return NhlNOERROR;
}

/*
 * Function:	nhl_ferrclearmsgs
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
void
_NHLCALLF(nhl_ferrclearmsgs,NHL_FERRCLEARMSGS)
#if	NhlNeedProto
(
	int	*ierr
)
#else
(ierr)
	int	*ierr;
#endif
{
	*ierr = NhlErrClearMsgs();

	return;
}

/*
 * Function:	NhlErrAddTable
 *
 * Description:	This function adds an error msg table to the error object.
 *		This is the messages that get printed out with respect to
 *		the errno reported to NhlPError function.
 *
 * In Args:	
 *		int	start,		starting number
 *		int	tlen,		table length
 *		char	**etable	table of err messages
 *
 * Out Args:	
 *
 * Scope:	Global Public
 * Returns:	NhlErrorTypes
 * Side Effect:	
 */
NhlErrorTypes
NhlErrAddTable
#if	NhlNeedProto
(
	int		start,		/* starting number		*/
	int		tlen,		/* table length			*/
	Const char	**etable	/* table of err messages	*/
)
#else
(start,tlen,etable)
	int		start;		/* starting number		*/
	int		tlen;		/* table length			*/
	Const char	**etable;	/* table of err messages	*/
#endif
{
	int i;
	unsigned end;
	unsigned tstart, tend;

	if(errorLayer == NULL){
		NHLPERROR((NhlFATAL,NhlEUNKNOWN,"Can't find Error Object"));
		return NhlFATAL;
	}

	/*
	 * make sure new table has a valid range
	 */
	for(i=0;i < errorLayer->error.num_etables;i++){
		end = start + tlen;
		tstart = errorLayer->error.etables[i].start;
		tend = tstart + errorLayer->error.etables[i].len;

		if(((start > tstart) && (start < tend)) ||
			   ((end > tstart) && (end < tend))){

			NHLPERROR((NhlFATAL,NhlEUNKNOWN,
				"Table being added overlaps a previous table"));
			return NhlFATAL;
		}
	}

	/*
	 * Increase size of table if needed
	 */
	if(errorLayer->error.len_etables < errorLayer->error.num_etables + 1){
		errorLayer->error.etables = (NhlETable *)
					NhlRealloc(errorLayer->error.etables,
				(unsigned)((errorLayer->error.len_etables +
					TABLELISTINC) * sizeof(NhlETable)));
		if(errorLayer->error.etables == NULL){
			errorLayer->error.len_etables = 0;
			errorLayer->error.num_etables = 0;

			NHLPERROR((NhlFATAL,errno,
				"Unable to allocate memory for error table"));
			return NhlFATAL;
		}
		else
			errorLayer->error.len_etables += TABLELISTINC;
	}

	/*
	 * Add table into list
	 */
	errorLayer->error.etables[errorLayer->error.num_etables].start = start;
	errorLayer->error.etables[errorLayer->error.num_etables].len = tlen;
	errorLayer->error.etables[errorLayer->error.num_etables].errs = etable;
	errorLayer->error.num_etables++;

	return NhlNOERROR;
}

/*
 * Function:	NhlErrSPrintMsg
 *
 * Description:	This function takes an error msg and formats it for printing
 *		It places the resulting string in the buffer provided.
 *
 * In Args:	
 *		Const NhlErrMsg	*msg		message to print
 *
 * Out Args:	
 *		char		*buffer,	buffer to print message to
 *
 * Scope:	Global Public
 * Returns:	Const char *
 * Side Effect:	
 */
char *
NhlErrSPrintMsg
#if	NhlNeedProto
(
	char		*buffer,	/* buffer to print message to	*/
	Const NhlErrMsg	*msg		/* message to print		*/
)
#else
(buffer,msg)
	char		*buffer;	/* buffer to print message to	*/
	Const NhlErrMsg	*msg;		/* message to print		*/
#endif
{
	char tbuf[MAXERRMSGLEN];

	if(msg->severity == NhlNOERROR)
		strcpy(buffer,"noerror");
	else if(msg->severity == NhlINFO)
		strcpy(buffer,"info");
	else if(msg->severity == NhlWARNING)
		strcpy(buffer,"warning");
	else
		strcpy(buffer,"fatal");
	
	if((msg->line > 0) && (msg->fname != NULL)){
		sprintf(tbuf,":[\"%s\":%d]",msg->fname,msg->line);
		strcat(buffer,tbuf);
	}

	if(msg->msg != NULL){
		strcat(buffer,":");
		strcat(buffer,msg->msg);
	}

	if(msg->errorno != NhlEUNKNOWN){
		sprintf(tbuf,":[errno=%d]",msg->errorno);
		strcat(buffer,tbuf);
	}

	if(msg->sysmsg != NULL){
		strcat(buffer,":");
		strcat(buffer,msg->sysmsg);
	}

	return buffer;
}

/*
 * Function:	nhl_ferrsprintmsg
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
void _NHLCALLF(nhl_ferrsprintmsg,NHL_FERRSPRINTMSG)
#if	NhlNeedProto
(
	_NhlFString	fbuffer,
	int		*fbuffer_len,
	int		*msg_num
)
#else
(fbuffer,fbuffer_len,msg_num)
	_NhlFString	fbuffer;
	int		*fbuffer_len;
	int		*msg_num;
#endif
{
	char		tbuf[MAXERRMSGLEN];
	char		*msgstr;
	Const NhlErrMsg	*msg;
	NhlErrorTypes	ret;

	ret = NhlErrGetMsg(*msg_num,&msg);

	if(ret < NhlWARNING)
		msgstr = NULL;
	else
		msgstr = NhlErrSPrintMsg(tbuf,msg);

	_NhlCstrToFstr(fbuffer,*fbuffer_len,msgstr);

	return;
}

/*
 * Function:	NhlErrFPrintMsg
 *
 * Description:	This function takes the given error msg and formats it for
 *		printing using NhlErrSPrintMsg.  And then prints it to
 *		the FILE* provided.
 *
 * In Args:	
 *		FILE		*fp;	file to print to
 *		Const NhlErrMsg	*msg	message to print
 *
 * Out Args:	
 *
 * Scope:	Global Public
 * Returns:	Const char *
 * Side Effect:	
 */
Const char *
NhlErrFPrintMsg
#if	NhlNeedProto
(
	FILE		*fp,	/* file to print to	*/
	Const NhlErrMsg	*msg	/* message to print	*/
)
#else
(fp,msg)
	FILE		*fp;	/* file to print to	*/
	Const NhlErrMsg	*msg;	/* message to print	*/
#endif
{
	static char tbuf[MAXERRMSGLEN];

	if(fprintf(fp,"%s\n\r",NhlErrSPrintMsg(tbuf,msg)) < 0)
		fprintf(stderr,"Unable to print Error Messages???");
	fflush(fp);

	return tbuf;
}

/*
 * Function:	nhl_fgerhnd
 *
 * Description:	This function will get called in the event of a GKS error.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
void _NHLCALLF(nhl_fgerhnd,NHL_FGERHND)
#if	NhlNeedProto
(
	_NhlFString	ffname,
	int		*ffname_len,
	_NhlFString	fmesg,
	int		*fmesg_len
)
#else
(ffname,ffname_len,fmesg,fmesg_len)
	_NhlFString	ffname;
	int		*ffname_len;
	_NhlFString	fmesg;
	int		*fmesg_len;
#endif
{
	int		err_num;
	char		stack_gks_func[_NhlGKSMAXMSGLEN];
	char		stack_gks_msg[_NhlGKSMAXMSGLEN];
	char		*gks_func,*gks_msg;

	/* reset libncarg error mode if it is currently set */
	if(c_nerro(&err_num)){
		gks_msg = c_semess(0);
		/*
		 * Was the error reported from this function?
		 * If not, don't reset the errorflag - this will cause
		 * a stop when seter is called, but It should only end
		 * up happening if one of the objects doesn't bother to
		 * check the error flag after calling libncarg/libncarg_gks
		 * functions.
		 */
		if((err_num == _NhlGKSERRNUM) &&
		!strncmp(gks_msg,_NhlGKSERRMSG,strlen(_NhlGKSERRMSG))){
			c_errof();
		}
	}

	gks_func = _NhlFstrToCstr(stack_gks_func,NhlNumber(stack_gks_func),
							ffname,*ffname_len);
	gks_msg = _NhlFstrToCstr(stack_gks_msg,NhlNumber(stack_gks_msg),
							fmesg,*fmesg_len);
	if(!gks_func)
		gks_func = "Unknown GKS Func";

	if(!gks_msg)
		gks_msg = "Unknown GKS Message";

	NhlPError(NhlWARNING,NhlEUNKNOWN,"GKS:%s:%s",gks_func,gks_msg);

	c_seter(_NhlGKSERRMSG,_NhlGKSERRNUM,1);

	return;
}
static NhlErrorTypes ErrorGetValues
#if	NhlNeedProto
(NhlLayer l, _NhlArgList args, int nargs)
#else
(l, args, nargs)
NhlLayer l;
_NhlArgList args;
int nargs;
#endif
{
	NhlErrorLayerPart *erp = &(((NhlErrorLayer)l)->error);
	NrmQuark Qerrfile = NrmStringToQuark(NhlNerrFileName);
	int i;

	for(i = 0; i < nargs; i++) {
		if(args[i].quark == Qerrfile) {	
			if(erp->error_file != NULL) {
			*((NhlString*)args[i].value.ptrval) = (NhlString)NhlMalloc(strlen(erp->error_file) +1);
				strcpy(*((NhlString*)args[i].value.ptrval),erp->error_file);
			} else {
				*((NhlString*)args[i].value.ptrval) = NULL;
			}
		}
	}
	return(NhlNOERROR);
}
