/*
 *      $Id: goP.h,v 1.14 2000-05-16 01:59:25 dbrown Exp $
 */
/************************************************************************
*									*
*			     Copyright (C)  1996			*
*	     University Corporation for Atmospheric Research		*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		goP.h
 *
 *	Author:		Jeff W. Boote
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Thu Sep 19 13:35:57 MDT 1996
 *
 *	Description:	
 */
#ifndef	_NG_GOP_H_
#define	_NG_GOP_H_

#ifdef DEBUG
#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>
#endif

#include <ncarg/hlu/BaseP.h>

#include <ncarg/ngo/ngoP.h>
#include <ncarg/ngo/go.h>

#include <ncarg/ngo/xapp.h>
#include <ncarg/ngo/varmenus.h>
#include <ncarg/ngo/createmenu.h>


#ifdef  DEBUG
#define XGRABSERVER(dpy)
#define XUNGRABSERVER(dpy)
#define XGRABPOINTER(dpy,win,own,emask,pmode,kmode,cwin,cursor,time) \
			GrabSuccess
#define XUNGRABPOINTER(dpy,time) 
#else
#define XGRABSERVER(dpy)        XGrabServer(dpy)
#define XUNGRABSERVER(dpy)      XUngrabServer(dpy)
#define XGRABPOINTER(dpy,win,own,emask,pmode,kmode,cwin,cursor,time) \
	XGrabPointer(dpy,win,own,emask,pmode,kmode,cwin,cursor,time)
#define XUNGRABPOINTER(dpy,time)	XUngrabPointer(dpy,time)
#endif

typedef struct _NgGOClassRec *NgGOClass;
typedef struct _NgGORec *NgGO;

typedef struct _NgGOPart {
/* resources */
	NhlString	title;
	NhlString	edit_field_color;
	NhlString	select_color;
	NhlBoolean	sensitive;
	

/* private fields */
	XmString	xm_title;
	int		appmgr;
        int		nclstate;

	NhlBoolean	x_sensitive;
	NhlBoolean	i_sensitive;
	_NhlCB		appdestroy_cb;

	NgXAppExport	x;

	Window		iowin;

	NhlBoolean	subshell;
	NhlBoolean	pup;
	Widget		pshell;

	NhlBoolean	up;
	Widget		shell;
	Xcb		xcb;
	Widget		manager;	/* subclasses must use this container */

	Widget		menubar;
	Widget		menush;
	Widget		fmenu;
	Widget		emenu;
	Widget		vmenu;
	Widget		omenu;
	Widget		wmenu,wsep1,wsep2;
	Widget		hmenu;
	Widget		file;
	Widget		edit;
	Widget		view;
	Widget		options;
	Widget		window;
	Widget		help;
	Widget		close;
        NgCreateMenu	*create_menu;
        NgVarMenus	delete_menu;
        _NhlCB		gochange_cb;
	XtTranslations	global_trans;
	XtTranslations	editing_trans;
	XtTranslations	tf_editing_trans;
	int		print;
	Pixel		edit_field_pixel;
	Pixel		select_pixel;
} NgGOPart;

typedef struct _NgGORec {
	NhlObjLayerPart	base;
	NgGOPart	go;
} NgGORec;

typedef NhlBoolean (*_NgGOFunc)(
	NgGO	go
);

#define _NgGOInheritClose	((_NgGOFunc)_NhlInherit)

typedef struct _NgGOClassPart {
	/*
	 * These three widgetclasses are settable in sub-classes in
	 * their ClassPartInitialize procedure.  The following defaults
	 * are set by the go class.  The first two must be sub-classes
	 * of the vendorShellWidget, and the manager must be a subclass
	 * of xmManager.  These constraints are not checked, but the
	 * go class will set resources to the objects as if it were one
	 * of these.
	 */
	WidgetClass	dialog;		/* transientShellWidgetClass	*/
	WidgetClass	toplevel;	/* applicationShellWidgetClass	*/
	WidgetClass	manager;	/* xmFormWidgetClass		*/

	NhlBoolean	top_win_chain;
	_NgGOFunc	create_win;		/* D chained	*/
	_NgGOFunc	create_win_hook;	/* D chained	*/
	_NgGOFunc	close;

} NgGOClassPart;

typedef struct _NgGOClassRec {
	NhlObjClassPart		base_class;
	NgGOClassPart		go_class;
} NgGOClassRec;

extern NgGOClassRec	NggOClassRec;

extern void _NgIGOSensitive(
	NgGO		go,
	NhlBoolean	sensitive
);

/*
 * Convenience functions to register a callback that will automatically
 * call NgGOPop{up,down}.  udata is goid.
 */
extern void _NgGOPopupCB(
	Widget		w,
	XtPointer	udata,
	XtPointer	cbdata
);

extern void _NgGOPopdownCB(
	Widget		w,
	XtPointer	udata,
	XtPointer	cbdata
);

/*
 * Automatic action calling routines.  This callback should be registered
 * on all menu buttons that call the standard actions:
 *	closeWindow
 *	quitApplication
 *
 * The buttons must be given the same name as the action.  If you want to
 * send parameters to the action routine, that is done by setting the
 * udata parameter of the CB to a NULL terminated string array.
 */
extern void _NgGODefActionCB(
	Widget		w,
	XtPointer	udata,
	XtPointer	cbdata
);

extern void _NgGOCreateMenubar(
	NgGO		go
);

extern void _NgGOSetTitle(
	NgGO		go,
	Const char	*title,
	Const char	*icon_title
);

extern void _NgGOWidgetTranslations(
	NgGO	go,
	Widget	w
);

#endif	/* _NG_GOP_H_ */
