/*
 *      $Id: browse.c,v 1.29 1999-07-30 03:20:44 dbrown Exp $
 */
/************************************************************************
*									*
*			     Copyright (C)  1996			*
*	     University Corporation for Atmospheric Research		*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		browse.c
 *
 *	Author:		David I. Brown
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Tue Mar  4 12:38:45 MST 1997
 *
 *	Description:	
 */
#include <ncarg/ngo/browseP.h>
#include <ncarg/ngo/nclstate.h>
#include <ncarg/ngo/xutil.h>
#include <ncarg/ngo/mwin.h>
#include <ncarg/ngo/filepageP.h>
#include <ncarg/ngo/varpageP.h>
#include <ncarg/ngo/hlupageP.h>
#include <ncarg/ngo/htmlpageP.h>
#include <ncarg/ngo/plotpageP.h>

#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/PanedW.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/Frame.h>
#include <Xm/List.h>
#include <Xm/ScrolledW.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/RowColumn.h>
#include <ncarg/ngo/CascadeBG.h>
#include <Xm/MenuShell.h>
#include <Xm/DrawnB.h>
#include <ncarg/ngo/Folder.h>

static void TabFocusAction(
#if	NhlNeedProto
	Widget		w,
	XEvent		*xev,
	String		*params,
	Cardinal	*num_params
#endif
);

static XtActionsRec browseactions[] = {
	{ "TabFocusAction", TabFocusAction }
};

static NhlErrorTypes BrowseClassPartInitialize(
        NhlClass        lc
);

static NhlErrorTypes BrowseInitialize(
	NhlClass	lc,
	NhlLayer	req,
	NhlLayer	new,
	_NhlArgList	args,
	int		nargs
);

static NhlErrorTypes BrowseDestroy(
	NhlLayer	l
);

static NhlBoolean BrowseCreateWin(
	NgGO	go
);

static brPage *
UpdatePanes(
	NgGO		go,
        brPageType	type,
        NrmQuark	qvar,
        NrmQuark	qfile,
	NhlBoolean	delete,
	NhlPointer	init_data
);

NgBrowseClassRec NgbrowseClassRec = {
	{
/* class_name		*/	"browseClass",
/* nrm_class		*/	NrmNULLQUARK,
/* layer_size		*/	sizeof(NgBrowseRec),
/* class_inited		*/	False,
/* superclass		*/	(NhlClass)&NggOClassRec,
/* cvt_table		*/	NULL,

/* layer_resources	*/	NULL,
/* num_resources	*/	0,
/* all_resources	*/	NULL,
/* callbacks		*/	NULL,
/* num_callbacks	*/	0,
/* class_callbacks	*/	NULL,
/* num_class_callbacks	*/	0,

/* class_part_initialize*/	BrowseClassPartInitialize,
/* class_initialize	*/	NULL,
/* layer_initialize	*/	BrowseInitialize,
/* layer_set_values	*/	NULL,
/* layer_set_values_hook*/	NULL,
/* layer_get_values	*/	NULL,
/* layer_reparent	*/	NULL,
/* layer_destroy	*/	BrowseDestroy,

	},
	{
/* dialog		*/	NULL,
/* toplevel		*/	NULL,
/* manager		*/	NULL,

/* top_win_chain	*/	False,
/* create_win		*/	BrowseCreateWin,
/* create_win_hook	*/	NULL,
/* close		*/	_NgGOInheritClose,
	},
	{
/* foo			*/	0,
	},
};

NhlClass NgbrowseClass = (NhlClass)&NgbrowseClassRec;

static NgPageId CurrentPageId = 0;

static NhlErrorTypes
BrowseClassPartInitialize
(
        NhlClass        lc
)
{
	NgBrowseClassRec *bclass = (NgBrowseClassRec *) lc;
	NgBrowseClassPart *bcp = &bclass->browse_class;

	bcp->hidden_page_state = NULL;
	bcp->page_messages = NULL;

	return NhlNOERROR;
}
static NhlErrorTypes
BrowseInitialize
(
	NhlClass	lc,
	NhlLayer	req,
	NhlLayer	new,
	_NhlArgList	args,
	int		nargs
)
{
	char			func[] = "BrowseInitialize";
	NgBrowse		browse = (NgBrowse)new;
	NgBrowsePart		*np = &((NgBrowse)new)->browse;
	NgBrowsePart		*rp = &((NgBrowse)req)->browse;

	np->nsid = NhlDEFAULT_APP;
	NhlVAGetValues(browse->go.appmgr,
		NgNappNclState,	&np->nsid,
		NULL);
	if(!NhlIsClass(np->nsid,NgnclStateClass)){
		NHLPERROR((NhlFATAL,NhlEUNKNOWN,"%s:Invalid nclstate resource",
									func));
		return NhlFATAL;
	}

        np->vmenus = NULL;
        
	return NhlNOERROR;
}

static NhlErrorTypes
BrowseDestroy
(
	NhlLayer	l
)
{
	NgBrowse	browse = (NgBrowse)l;
	NgBrowsePart	*np = &((NgBrowse)l)->browse;

#if 0        
	NgAppRemoveGO(browse->go.appmgr,l->base.id);
#endif
	return NhlNOERROR;
}

#if	NOT
static Widget
CreateMenuBar
(
	NgGO	go
)
{
	NgBrowsePart	*np = &((NgBrowse)go)->browse;
	Widget		menubar,menush,fmenu,emenu;
	Widget		vmenu,omenu,wmenu,hmenu;
	Widget		file,edit,view,options,window,help;
	Widget		addfile,load,close,quit,ncledit;
        
	menubar = XtVaCreateManagedWidget
                ("menubar",xmRowColumnWidgetClass,
                 go->go.manager,
                 XmNrowColumnType,	XmMENU_BAR,
                 NULL);

	menush = XtVaCreatePopupShell
                ("menush",xmMenuShellWidgetClass,
                 go->go.shell,
		XmNwidth,		5,
		XmNheight,		5,
		XmNallowShellResize,	True,
		XtNoverrideRedirect,	True,
		XmNdepth,		XcbGetDepth(go->go.xcb),
		XmNcolormap,		XcbGetColormap(go->go.xcb),
		XmNvisual,		XcbGetVisual(go->go.xcb),
		NULL);
	fmenu = XtVaCreateWidget
                ("fmenu",xmRowColumnWidgetClass,menush,
                 XmNrowColumnType,	XmMENU_PULLDOWN,
                 NULL);

	emenu = XtVaCreateWidget
                ("emenu",xmRowColumnWidgetClass,menush,
                 XmNrowColumnType,	XmMENU_PULLDOWN,
                 NULL);

	vmenu = XtVaCreateWidget
                ("vmenu",xmRowColumnWidgetClass,menush,
                 XmNrowColumnType,	XmMENU_PULLDOWN,
                 NULL);

	omenu = XtVaCreateWidget
                ("omenu",xmRowColumnWidgetClass,menush,
                 XmNrowColumnType,	XmMENU_PULLDOWN,
                 NULL);

	wmenu = XtVaCreateWidget
                ("wmenu",xmRowColumnWidgetClass,menush,
                 XmNrowColumnType,	XmMENU_PULLDOWN,
                 NULL);

	hmenu = XtVaCreateWidget
                ("hmenu",xmRowColumnWidgetClass,menush,
                 XmNrowColumnType,	XmMENU_PULLDOWN,
                 NULL);

	file = XtVaCreateManagedWidget
                ("file",xmCascadeButtonGadgetClass,
                 menubar,
                 XmNsubMenuId,	fmenu,
                 NULL);

	edit = XtVaCreateManagedWidget
                ("edit",xmCascadeButtonGadgetClass,
                 menubar,
                 XmNsubMenuId,	emenu,
                 NULL);

	view = XtVaCreateManagedWidget
                ("view",xmCascadeButtonGadgetClass,
                 menubar,
                 XmNsubMenuId,	vmenu,
                 NULL);

	options = XtVaCreateManagedWidget
                ("options",xmCascadeButtonGadgetClass,
                 menubar,
                 XmNsubMenuId,	omenu,
                 NULL);

	window = XtVaCreateManagedWidget
                ("window",xmCascadeButtonGadgetClass,
                 menubar,
                 XmNsubMenuId,	wmenu,
                 NULL);

	help = XtVaCreateManagedWidget
                ("help",xmCascadeButtonGadgetClass,
                 menubar,
                 XmNsubMenuId,	hmenu,
                 NULL);

	XtVaSetValues
                (menubar,
                 XmNmenuHelpWidget,	help,
                 NULL);

	addfile = XtVaCreateManagedWidget
                ("addFile",xmPushButtonGadgetClass,
                 fmenu,
                 NULL);
	XtAddCallback(addfile,XmNactivateCallback,_NgGODefActionCB,NULL);

	load = XtVaCreateManagedWidget
                ("loadScript",xmPushButtonGadgetClass,
                 fmenu,
                 NULL);
	XtAddCallback(load,XmNactivateCallback,_NgGODefActionCB,NULL);

	close = XtVaCreateManagedWidget
                ("closeWindow",xmPushButtonGadgetClass,
                 fmenu,
                 NULL);
	XtAddCallback(close,XmNactivateCallback,_NgGODefActionCB,NULL);

	quit = XtVaCreateManagedWidget
                ("quitApplication",xmPushButtonGadgetClass,
                 fmenu,
                 NULL);
	XtAddCallback(quit,XmNactivateCallback,_NgGODefActionCB,NULL);
        
	ncledit = XtVaCreateManagedWidget
                ("nclWindow",xmPushButtonGadgetClass,
                 wmenu,
                 NULL);
	XtAddCallback(ncledit,XmNactivateCallback,_NgGODefActionCB,NULL);

	XtManageChild(fmenu);
	XtManageChild(emenu);
	XtManageChild(vmenu);
	XtManageChild(omenu);
	XtManageChild(wmenu);
	XtManageChild(hmenu);

        return menubar;
        
}
#endif

/*
 * Sets the folder size to the greater of the available space
 * in the clip window and the size of the page. It returns the size of
 * the available space for page elements that should be sized to fit.
 */

extern void
NgSetFolderSize
(
	brPane	*pane,
        Dimension page_width,
        Dimension page_height,
        Dimension *avail_width,
        Dimension *avail_height
        )
{
	int			left_off,top_off,right_off,bottom_off;
	Dimension		tab_height;
        Dimension		sw_width,sw_height;
        Dimension		vscroll_width = 23, hscroll_height = 23;
        Dimension		folder_border = 2,clip_border = 2;
        Dimension		req_width,folder_width, folder_height;
        enum SB_Status { Yes, No, Maybe } h_scroll_status, v_scroll_status;
        
	XtVaGetValues(pane->folder,
		      XmNtabBarHeight,&tab_height,
		      XmNtopOffset,&top_off,
		      XmNleftOffset,&left_off,
		      XmNrightOffset,&right_off,
		      XmNbottomOffset,&bottom_off,
		      NULL);
        
        XtVaGetValues(pane->scroller,
                      XmNwidth, &sw_width,
                      XmNheight, &sw_height,
                      NULL);
        *avail_width = sw_width - left_off - right_off
                - 2 * folder_border - 2 * clip_border;
        *avail_height = sw_height - top_off - bottom_off
                - 2 * folder_border - 2 * clip_border - tab_height;

        req_width = MAX(page_width,pane->max_tab_xtnt);
        if (req_width > *avail_width)
                h_scroll_status = Yes;
        else if (req_width < *avail_width - vscroll_width)
                h_scroll_status = No;
        else
                h_scroll_status = Maybe;
        
        if (page_height > *avail_height)
                v_scroll_status = Yes;
        else if (page_height < *avail_height - hscroll_height)
                v_scroll_status = No;
        else
                v_scroll_status = Maybe;

        if (v_scroll_status == Maybe && h_scroll_status == Maybe)
                v_scroll_status = h_scroll_status = No;
        else if (v_scroll_status == Maybe)
                v_scroll_status = h_scroll_status;
        else if (h_scroll_status == Maybe)
                h_scroll_status = v_scroll_status;

        if (v_scroll_status == Yes)
                *avail_width -= vscroll_width;
        if (h_scroll_status == Yes)
                *avail_height -= hscroll_height;

        folder_width = MAX(*avail_width + 2 * folder_border,
                           req_width + 2 * folder_border);
        folder_height = MAX(*avail_height + 2 * folder_border + tab_height,
                            page_height + 2 * folder_border + tab_height + 2);
        
        XtVaSetValues(pane->folder,
                      XmNwidth,folder_width,
                      XmNheight,folder_height,
                      NULL);
        
        return;
}

static char *PageString
(
        brPane		*pane,
        brPage		*page,
	NhlBoolean	abbrev
        )
{
        static char string[512];
        
        switch (page->type) {
	    default:
                    strcpy(string,NrmQuarkToString(page->qvar));
                    break;
	    case _brHTML:
                    strcpy(string,"Help");
                    break;
            case _brFILEREF:
                    strcpy(string,NrmQuarkToString(page->qfile));
                    break;
            case _brFILEVAR:
		    if (abbrev) {
			    strcpy(string,"->");
		    }
		    else {
			    strcpy(string,NrmQuarkToString(page->qfile));
			    strcat(string,"->");
		    }
                    strcat(string,NrmQuarkToString(page->qvar));
                    break;
	    case _brNULL:
		    strcpy(string,"<null>");
		    break;
        }

        return string;
        
}
static void SetTabFocus
(
        NgGO	go,
        brPane	*pane,
        brPage	*page
        )
{
	NgBrowse	browse = (NgBrowse)go;
	NgBrowsePart	*np = &browse->browse;
        brPaneControl	*pcp = &np->pane_ctrl;
        XFocusChangeEvent xev;
        int		pos;
        
        for (pos = 0; pos < pane->pagecount; pos++) {
                brPage *tpage = XmLArrayGet(pane->pagelist,pos);
                if (tpage && tpage == page)
                        break;
        }
        
	if (pos == pane->pagecount) {
#if	DEBUG_DATABROWSER & DEBUG_FOLDER
                fprintf(stderr,"page not found\n");
#endif
                return;
	}

#if	DEBUG_DATABROWSER & DEBUG_FOLDER
        fprintf(stderr,"focus: %s tab #%d\n",PageString(pane,page,False),pos);
#endif
        XtSetKeyboardFocus(go->go.manager,page->tab->tab);
        xev.type = FocusIn;
        xev.send_event = False;
        xev.display = XtDisplay(page->tab->tab);
        xev.window = XtWindow(page->tab->tab);
        xev.mode = NotifyNormal;
        xev.detail = NotifyDetailNone;
        XtCallActionProc(page->tab->tab,
                         "XmLFolderPrimFocusIn",(XEvent*)&xev,NULL,0);
        
        pcp->focus_pane = pane;
        pcp->focus_pos = pos;
        
        return;
}

static void ActiveTabCB 
(
	Widget		w,
	XtPointer	udata,
	XtPointer	cb_data
)
{
        brPane		*pane = (brPane *)udata;
	NgGO		go = pane->go;
	NgBrowse	browse = (NgBrowse)go;
	NgBrowsePart	*np = &browse->browse;
        brPaneControl	*pcp = &np->pane_ctrl;
        brPage		*page,*oldpage = NULL;
        int		i;
        char		*string;
        
        XmLFolderCallbackStruct *cbs = (XmLFolderCallbackStruct *)cb_data;

        
#if	DEBUG_DATABROWSER & DEBUG_ENTRY
	fprintf(stderr,"ActiveTabCB(IN)\n");
#endif
        
	if (cbs->pos >= pane->pagecount) {
#if	DEBUG_DATABROWSER & DEBUG_FOLDER
                fprintf(stderr,"tab position out of range\n");
#endif
                return;
	}
        page = XmLArrayGet(pane->pagelist,cbs->pos);
        if (pane->active_page)
                oldpage = XmLArrayGet(pane->pagelist,pane->active_pos);

        if (pane->active_page && pane->active_page != page)
                NgReleasePageHtmlViews(go->base.id,pane->active_page->id);

#if	DEBUG_DATABROWSER & DEBUG_FOLDER
        string = PageString(pane,page,False);
        fprintf(stderr,"tab # %d activated for %s\n",cbs->pos,string);
#endif
        pane->active_pos = cbs->pos;
        pane->active_page = page;

#if	DEBUG_DATABROWSER & DEBUG_FOLDER
        fprintf(stderr,"setting keyboard focus to %s\n",
                       PageString(pane,page,False));
#endif

        SetTabFocus(go,pane,page);
        if (page->pdata->page_focus_notify)
                (*page->pdata->page_focus_notify)(page,True);
	(*page->pdata->adjust_page_geo)(page);
        
        return;
        
}

static void
TabFocusEH
(
	Widget		w,
	XtPointer	udata,
	XEvent		*event,
	Boolean		*cont
)
{
        brPane		*pane = (brPane *)udata;
	NgGO		go = pane->go;
	NgBrowse	browse = (NgBrowse)go;
	NgBrowsePart	*np = &browse->browse;
        brPaneControl	*pcp = &np->pane_ctrl;
        brPage		*page;
        int		pos;
        char		*string;
        

	switch (event->type) {
	case FocusOut:
		return;
	case FocusIn:
		break;
	}

#if	DEBUG_DATABROWSER & DEBUG_ENTRY
	fprintf(stderr,"TabFocusEH(IN)\n");
#endif
        for (pos = 0; pos < pane->pagecount; pos++) {
                page = XmLArrayGet(pane->pagelist,pos);
                if (page && page->tab && w == page->tab->tab)
                        break;
        }
        
	if (pos == pane->pagecount) {
#if	DEBUG_DATABROWSER & DEBUG_FOLDER
                fprintf(stderr,"tab position out of range\n");
#endif
                return;
	}

        string = PageString(pane,page,False);
#if	DEBUG_DATABROWSER & DEBUG_FOLDER
        fprintf(stderr,"focus: %s tab #%d\n",string,pos);
#endif

        pcp->focus_pane = pane;
        pcp->focus_pos = pos;
        
	return;
}

static void
CreateFolder
(
	NgGO		go,
        brPane	 	*pane
)
{
        WidgetList	children;
        int		i,num_children;
#if 0
	int		value,size,inc,page_inc,min,max;
	char *name;

	XtVaGetValues(pane->vsb,
		      XmNminimum,&min,
		      XmNmaximum,&max,
		      NULL);
	XtVaSetValues(pane->vsb,
		      XmNminimum,0,
		      XmNmaximum,100,
		      NULL);
	XmScrollBarGetValues(pane->vsb,&value,&size,&inc,&page_inc);
	XmScrollBarSetValues(pane->vsb,value,MAX(size,1),MAX(inc,1),
			     MAX(page_inc,1));
	name = XtName(pane->vsb);

	XSync(go->go.x->dpy,False);
	XSynchronize(go->go.x->dpy,True);
#endif
	XtManageChild(pane->form);
#if 0
	XSynchronize(go->go.x->dpy,False);
	XtManageChild(pane->vsb);
#endif

        pane->folder = XtVaCreateManagedWidget
                ("Folder",xmlFolderWidgetClass,
                 pane->form,
                 XmNresizePolicy,XmRESIZE_NONE,
                 XmNresizable,True,
                 XmNbottomAttachment,XmATTACH_NONE,
                 XmNrightAttachment,XmATTACH_NONE,
		 XmNheight,10,
		 XmNwidth,10,
                 NULL);
#if	DEBUG_DATABROWSER & DEBUG_FOLDER
        fprintf(stderr,"folder %x pane %x\n", pane->folder,pane);
        XtVaGetValues(pane->form,
                      XmNchildren,&children,
                      XmNnumChildren,&num_children,
                      NULL);
        for (i = 0; i < num_children; i++) {
                fprintf(stderr,"Form child %s\n",XtName(children[i]));
        }
#endif
        
        XtAddCallback(pane->folder,XmNactivateCallback,ActiveTabCB,pane);

	XtVaCreateManagedWidget("BogusWidget",
				xmLabelGadgetClass,pane->folder,
				NULL);
	pane->has_folder = True;
	_NgGOWidgetTranslations(go,pane->folder);


	return;
}
static void
DestroyPageDataList
(
	NgGO		go,
        brPageDataList	pdata
)
{
	brPageData	*pd,*next;

	for (pd = pdata; pd != NULL ; ) {
		if (pd->dl)
			NclFreeDataList(pd->dl);
		(*pd->destroy_page)(pd->type_rec);
		XtDestroyWidget(pd->form);
		next = pd->next;
		NhlFree(pd);
		pd = next;
	}
	return;
}

static void
DestroyFolder
(
	NgGO		go,
        brPane	 	*pane
)
{
	int i;

	DestroyPageDataList(go,pane->var_pages);
	DestroyPageDataList(go,pane->fileref_pages);
	DestroyPageDataList(go,pane->hlu_pages);
	DestroyPageDataList(go,pane->html_pages);
	DestroyPageDataList(go,pane->plot_pages);
	for (i = 0; i < pane->tabcount; i++) {
		brTab *tab = XmLArrayGet(pane->tablist,i);
		XtDestroyWidget(tab->tab);
		NhlFree(tab);
	}
	XmLArrayFree(pane->tablist);
	for (i = 0; i < pane->pagecount; i++) {
		brPage *page = XmLArrayGet(pane->pagelist,i);
		NhlFree(page);
	}
	XmLArrayFree(pane->pagelist);

	XtDestroyWidget(pane->folder);
	pane->has_folder = False;
        pane->tabcount = 0;
        pane->tablist = NULL;
	pane->pagelist = NULL;
        pane->last_pagecount = pane->pagecount = 0;
	pane->fileref_pages = NULL;
	pane->var_pages = NULL;
	pane->hlu_pages = NULL;
	pane->html_pages = NULL;
	pane->plot_pages = NULL;
        pane->active_pos = 0;
}

static Cardinal FolderFirstProc
(
        Widget form_child
        )
{
        int num_children;
        
        if (XtClass(form_child) == xmlFolderWidgetClass)
                return 0;
        XtVaGetValues(XtParent(form_child),
                      XmNnumChildren,&num_children,
                      NULL);
        return num_children;
}

static void
vsbCB
(
	Widget w, 
	XtPointer udata, 
	XtPointer data
)
{
        brPane	 	*pane = (brPane *) udata;
#if DEBUG_DATABROWSER
	fprintf(stderr,"in vsb CB\n");
#endif

	if (! pane)
		return;
	if (! pane->active_page)
		return;

	if (pane->active_page->type == _brHTML)
		NgRefreshHtmlPage(pane->active_page);

	return;
}

static void
InitPane
(
	NgGO		go,
        Widget		parent,
        brPane	 	*pane
)
{
        XtActionList actions;
        Cardinal num_actions;
	int n;
	Arg args[2];
        
        pane->go = go;
        pane->managed = False;
        pane->topform = XtVaCreateWidget
                ("topform", xmFormWidgetClass,parent,
		 XmNallowResize,	True,
                 NULL);
	n = 0;
	XtSetArg(args[n],XmNscrollBarDisplayPolicy,XmAS_NEEDED);n++;
	XtSetArg(args[n],XmNscrollingPolicy,XmAUTOMATIC);n++;
	pane->scroller =
		XmCreateScrolledWindow(pane->topform,"scroller",args,n);

        XtVaGetValues(pane->scroller,
                      XmNclipWindow,&pane->clip_window,
                      XmNhorizontalScrollBar,&pane->hsb,
                      XmNverticalScrollBar,&pane->vsb,
                      NULL);
	XtAddCallback(pane->vsb, XmNvalueChangedCallback, 
		(XtCallbackProc)vsbCB,pane);


	XtManageChild(pane->scroller);
        pane->form = XtVaCreateWidget
                ("form", xmFormWidgetClass,pane->scroller,
                 XmNinsertPosition,FolderFirstProc,
                 NULL);

        pane->has_folder = False;

        pane->tabcount = 0;
        pane->tablist = NULL;
        pane->max_tab_xtnt = 0;
	pane->pagelist = NULL;
        pane->last_pagecount = pane->pagecount = 0;
	pane->fileref_pages = NULL;
	pane->var_pages = NULL;
	pane->hlu_pages = NULL;
	pane->html_pages = NULL;
	pane->plot_pages = NULL;
        pane->active_pos = 0;
        pane->active_page = NULL;
	pane->htmlview_count = 0;
	pane->htmlview_list = NULL;

        return;
}

static void
MapFirstPaneEH
(
	Widget		w,
	XtPointer	udata,
	XEvent		*event,
	Boolean		*cont
)
{
	brPane		*pane = (brPane *) udata;
	Dimension	height;

	if(event->type != MapNotify)
		return;

	XtRemoveEventHandler(w,StructureNotifyMask,False,MapFirstPaneEH,NULL);
	XtManageChild(pane->topform);

	return;
}

static brPane *
AddPane
(
	NgGO	go
)
{
	NgBrowse	browse = (NgBrowse)go;
	NgBrowsePart	*np = &browse->browse;
        NhlString	e_text;
        brPaneControl	*pcp = &np->pane_ctrl;
        brPane		*pane;
        int		i;
        Dimension	height,theight = 0;
        
/*
 * Temporarily fix the shell widget size,
 * but only if the shell has been realized
 */
        if (pcp->current_count > 0) {
                XtVaSetValues(go->go.manager,
                              XmNresizePolicy,XmRESIZE_NONE,
                              NULL);
        }
        
        for (i=0; i < pcp->current_count; i++) {
                XtVaGetValues(pcp->panes[i]->topform,
                              XmNheight,&height,
                              NULL);
                theight += height;
        }

        if (pcp->current_count < pcp->alloc_count) {
                pane = pcp->panes[pcp->current_count];
        }
        else {
                pcp->panes[pcp->current_count] = NhlMalloc(sizeof(brPane));
                if (!pcp->panes[pcp->current_count]) {
                        e_text = "%s: dynamic memory allocation error";
                        NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,_NhlName(go));
                }
                pane = pcp->panes[pcp->current_count];
                pcp->alloc_count++;
                InitPane(go,np->paned_window,pane);
        }
        pcp->current_count++;
        
        if(XtIsRealized(np->paned_window))
        	XtManageChild(pane->topform);
	else{
		XtAddEventHandler(np->paned_window,StructureNotifyMask,
			False,MapFirstPaneEH,pane);
	}
        pane->managed = True;
	_NgGOWidgetTranslations(go,pane->topform);

        height = theight / pcp->current_count;
	if(height > 0){
                for (i=0; i < pcp->current_count; i++) {
                        XtVaSetValues(pcp->panes[i]->topform,
                                      XmNpaneMaximum,height,
                                      XmNpaneMinimum,height,
                                      XmNheight,height,
                                      NULL);
                }
        }
        
        pcp->current_ix = pcp->current_count - 1;

        for (i=0; i < pcp->current_count; i++) {
                XtVaSetValues(pcp->panes[i]->topform,
                              XmNpaneMaximum,1000,
                              XmNpaneMinimum,1,
                              NULL);
        }

        XtVaSetValues(go->go.manager,
                      XmNresizePolicy,XmRESIZE_ANY,
                      NULL);
        return pane;
}

static void
RemovePane
(
	NgGO	go
)
{
	NgBrowse	browse = (NgBrowse)go;
	NgBrowsePart	*np = &browse->browse;
        NhlString	e_text;
        brPaneControl	*pcp = &np->pane_ctrl;
        brPane		*pane;
        int		i;
        Dimension	height,theight = 0;

        if (pcp->current_count < 2)
                return;

        XtVaSetValues(go->go.manager,
                      XmNresizePolicy,XmRESIZE_NONE,
                      NULL);
        for (i=0; i < pcp->current_count; i++) {
                XtVaGetValues(pcp->panes[i]->topform,
                              XmNheight,&height,
                              NULL);
                theight += height;
        }
        
        pane = pcp->panes[pcp->current_count-1];
        XtUnmanageChild(pane->topform);
        pane->managed = False;

        pcp->current_count--;
        if (pcp->current_ix >= pcp->current_count-1)
                pcp->current_ix = 0;
        
        height = theight / pcp->current_count;

        if (height > 0) {
                for (i=0; i < pcp->current_count; i++) {
                        XtVaSetValues(pcp->panes[i]->topform,
                                      XmNpaneMaximum,height,
                                      XmNpaneMinimum,height,
                                      XmNheight,height,
                                      NULL);
                }
        }

        for (i=0; i < pcp->current_count; i++) {
                XtVaSetValues(pcp->panes[i]->topform,
                              XmNpaneMaximum,1000,
                              XmNpaneMinimum,1,
                              NULL);
        }

        XtVaSetValues(go->go.manager,
                      XmNresizePolicy,XmRESIZE_ANY,
                      NULL);
        return;
}

static void
NextPane
(
	NgGO	go
        )
{
        
	NgBrowse	browse = (NgBrowse)go;
	NgBrowsePart	*np = &browse->browse;
        brPaneControl	*pcp = &np->pane_ctrl;

        pcp->current_ix = (pcp->current_ix+1) % pcp->current_count;
        return;
        
}

static brPane *
CurrentPane
(
	NgGO	go
)
{
	NgBrowse	browse = (NgBrowse)go;
	NgBrowsePart	*np = &browse->browse;
        brPaneControl	*pcp = &np->pane_ctrl;

        return pcp->panes[pcp->current_ix];
        
}

static int
PaneNumber
(
	NgGO	go,
	brPane	*pane
)
{
	NgBrowse	browse = (NgBrowse)go;
	NgBrowsePart	*np = &browse->browse;
        brPaneControl	*pcp = &np->pane_ctrl;
	int		i;

	for (i=0; i < pcp->alloc_count; i++) {
		if (pane == pcp->panes[i]) {
			return i;
		}
	}

        return -1;
        
}


static void
NewTab
(
	NgGO		go,
        brPane		*pane
)
{
	NgBrowse		browse = (NgBrowse)go;
	NgBrowsePart		*np = &browse->browse;
        NhlString		e_text;
        brTab			*tp,*tlp;
        int			pos = 0;

	if (!(tp = NhlMalloc(sizeof(brTab)))) {
		e_text = "%s: dynamic memory allocation error";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,_NhlName(go));
	}
	tp->tab =  XtVaCreateWidget
		("tab",xmDrawnButtonWidgetClass,pane->folder,
                 XmNuserData,pane,
		 NULL);
        XtAddEventHandler(tp->tab,FocusChangeMask,
                          False,TabFocusEH,(XtPointer)pane);
	_NgGOWidgetTranslations(go,tp->tab);
        
        tp->managed = False;

        if (! pane->tablist) {
                pane->tablist = XmLArrayNew(0,0);
        }
        else {
                pos = pane->tabcount;
        }
        XmLArrayAdd(pane->tablist,pos,1);
        XmLArraySet(pane->tablist,pos,tp);
        pane->tabcount++;
}

typedef enum _TabUpdateMode 
{
        _ADD,
        _REMOVE,
        _SHUFFLE
} TabUpdateMode;
        
static void
UpdateTabs
(
	NgGO		go,
        brPane		*pane,
        int		pos,
        TabUpdateMode	mode
)
{
	NgBrowse		browse = (NgBrowse)go;
	NgBrowsePart		*np = &browse->browse;
        brPaneControl		*pcp = &np->pane_ctrl;
        NhlString		e_text;
	char			*name;
	XmString		xmname;
        brPage			*page;
        NrmQuark		qfile = NrmNULLQUARK;
        int			count = 0;
        Dimension		x,width,tab_x,tab_width,form_width;
        brTab			*tab;
        int			i;

        if (mode == _REMOVE) {
                if (pane->active_pos == pane->remove_pos) {
                        if (pane->active_pos < pane->pagecount) {
#if	DEBUG_DATABROWSER & DEBUG_FOLDER
				fprintf(stderr,
					"setting tab %d active for pane %d\n",
					pane->pagecount,PaneNumber(go,pane));
#endif
                                XmLFolderSetActiveTab(
                                        pane->folder,pane->pagecount,False);
                        }
                        else {
#if	DEBUG_DATABROWSER & DEBUG_FOLDER
				fprintf(stderr,
					"setting tab %d active for pane %d\n",
					pane->pagecount-1,PaneNumber(go,pane));
#endif
                                XmLFolderSetActiveTab(
                                        pane->folder,pane->pagecount-1,False);
                        }
                }
                else {
#if	DEBUG_DATABROWSER & DEBUG_FOLDER
		        fprintf(stderr,
				"setting tab %d active for pane %d\n",
				pane->pagecount,PaneNumber(go,pane));
#endif
                        XmLFolderSetActiveTab(
                                pane->folder,pane->pagecount,False);
                }
                        
        }
        
        while (pane->tabcount < pane->pagecount) {
                NewTab(go,pane);
        }
        
        for (i = 0; i < pane->pagecount; i++) {
		NhlBoolean abbrev;
                tab = XmLArrayGet(pane->tablist,i);
                page = XmLArrayGet(pane->pagelist,i);
                
                switch (page->type) {
		default: 
			name = PageString(pane,page,False);
			break;
		case _brFILEREF:
			qfile = page->qfile;
			name = PageString(pane,page,False);
			break;
		case _brFILEVAR:
			if (page->qfile == qfile)
				name = PageString(pane,page,True);
			else	
				name = PageString(pane,page,False);
			break;
		}
                xmname = NgXAppCreateXmString(go->go.appmgr,name);
		XtVaSetValues(tab->tab,
			      XmNlabelString,xmname,
                              XmNtabManagedName,NULL,
			      XmNtabManagedWidget,page->pdata->form,
			      NULL);
		NgXAppFreeXmString(go->go.appmgr,xmname);

                if (! tab->managed) {
                        XtManageChild(tab->tab);
                        tab->managed = True;
                }
                page->tab = tab;
	}

        for (i = pane->pagecount; i < pane->tabcount; i++) {
                tab = XmLArrayGet(pane->tablist,i);
                XtVaSetValues(tab->tab,
                              XmNtabManagedName,"BogusWidget",
                              NULL);
        }
        if (mode == _REMOVE) {
                if (pane->active_pos == pane->remove_pos)
                        pos = 0;
                else if (pane->active_pos < pane->remove_pos)
                        pos = MAX(0,pane->active_pos);
                else
                        pos = MAX(0,pane->active_pos-1);
        }
        else if (pane->pagecount > 1) {
                int tmppos = (pos == 0) ? pane->pagecount-1 : 0;
                
#if	DEBUG_DATABROWSER & DEBUG_FOLDER
                fprintf(stderr,"setting tab %d active for pane %d\n",
                        pane->pagecount-1,PaneNumber(go,pane));
#endif
                XmLFolderSetActiveTab(pane->folder,tmppos,False);
        }
#if	DEBUG_DATABROWSER & DEBUG_FOLDER
	fprintf(stderr,
		"setting tab %d active for pane %d\n",
		pos,PaneNumber(go,pane));
#endif
        XmLFolderSetActiveTab(pane->folder,pos,True);
        pane->active_pos = pos;
        
        for (i = pane->pagecount; i < pane->tabcount; i++) {
                tab = XmLArrayGet(pane->tablist,i);
                XtUnmanageChild(tab->tab);
                tab->managed = False;
        }
/*
 * find the maximum tab x positions
 */
        tab = XmLArrayGet(pane->tablist,pane->pagecount-1);

        XtVaGetValues(tab->tab,
                      XmNwidth,&tab_width,
                      XmNx,&tab_x,
                      NULL);
        
        pane->max_tab_xtnt = tab_x + tab_width;
        
	return;
}

static int
InsertPage
(
        brPane		*pane,
        brPage		*page
        )
{
        int startpos = -1,endpos;
        int i,pos = 0;
        brPage *pp;
        
        if (! pane->pagelist) {
                pane->pagelist = XmLArrayNew(0,0);
                XmLArrayAdd(pane->pagelist,pos,1);
		pane->pagecount++;
                XmLArraySet(pane->pagelist,pos,page);
                return pos;
        }

        switch (page->type) {
	    default:
                    XmLArrayAdd(pane->pagelist,pos,1);
		    pane->pagecount++;
                    XmLArraySet(pane->pagelist,pos,page);
                    break;
            case _brFILEREF:
                    XmLArrayAdd(pane->pagelist,pos,1);
		    pane->pagecount++;
                    XmLArraySet(pane->pagelist,pos,page);
                    for (i = 1; i < pane->pagecount; i++) {
                            pp = XmLArrayGet(pane->pagelist,i);
                            if (page->qfile == pp->qfile) {
                                    if (startpos == -1) {
                                            startpos = endpos = i;
                                            continue;
                                    }
                                    endpos = i;
                            }
                            else if (startpos != -1) {
                                    break;
                            }
                    }
                    if (startpos > 1)
                            XmLArrayMove(pane->pagelist,
                                         1,startpos,endpos-startpos+1);
                    break;
            case _brFILEVAR:
                    for (i = 0; i < pane->pagecount; i++) {
                            pp = XmLArrayGet(pane->pagelist,i);
                            if (page->qfile == pp->qfile) {
                                    if (startpos == -1) {
                                            if (pp->type == _brFILEREF)
                                                    pos = 1;
                                            startpos = endpos = i;
                                            continue;
                                    }
                                    endpos = i;
                            }
                            else if (startpos != -1) {
                                    break;
                            }
                    }
                    if (startpos > 0)
                            XmLArrayMove(pane->pagelist,
                                         0,startpos,endpos-startpos+1);
                    XmLArrayAdd(pane->pagelist,pos,1);
		    pane->pagecount++;
                    XmLArraySet(pane->pagelist,pos,page);
        }


        return pos;
}

static void
DeleteVarPage
(
	brPage	*page
)
{
        NrmQuark	qvar,qfile;
	NgGO		go;

	go = (NgGO) page->go;
	UpdatePanes(go,page->type,page->qvar,page->qfile,True,NULL);
}

static void
VarDeleteCB
(
	NhlArgVal	cbdata,
	NhlArgVal	udata
)
{
	NgGO go = (NgGO) udata.ptrval;
        NgNclAny node = (NgNclAny)cbdata.ptrval;

#if	DEBUG_DATABROWSER
        fprintf(stderr,"deleting %s\n", node->name);
#endif        
        UpdatePanes(go,_brREGVAR,
                    NrmStringToQuark(node->name),NrmNULLQUARK,True,NULL);
                
	return;
}

static void
FileRefDeleteCB
(
	NhlArgVal	cbdata,
	NhlArgVal	udata
)
{
	NgGO		go = (NgGO) udata.ptrval;
	NgBrowse	browse = (NgBrowse)go;
        NgNclAny	node = (NgNclAny)cbdata.ptrval;
	NgBrowsePart	*np = &browse->browse;
        brPaneControl	*pcp = &np->pane_ctrl;
        brPane		*pane;
        brPage		*page;
        int		i,j;
        NrmQuark	qfile;

#if	DEBUG_DATABROWSER
        fprintf(stderr,"deleting %s\n", node->name);
#endif        
        qfile = NrmStringToQuark(node->name);
         
        for (i = 0; i < pcp->alloc_count; i++) {
                pane = pcp->panes[i];
                for (j = 0; j < pane->pagecount; ) {
                        page = XmLArrayGet(pane->pagelist,j);
                        if (page->qfile == qfile) {
                                if (page->type == _brFILEREF)
                                        UpdatePanes(go,_brFILEREF,
                                                    NrmNULLQUARK,
						    qfile,True,NULL);
                                else if (page->type == _brFILEVAR)
                                        UpdatePanes(go,_brFILEVAR,
                                                    page->qvar,
						    qfile,True,NULL);
                                continue;
                        }
                        j++;
                }
        }

	return;
}

static void
HluVarDeleteCB
(
	NhlArgVal	cbdata,
	NhlArgVal	udata
)
{
	NgGO go = (NgGO) udata.ptrval;
        NgNclAny node = (NgNclAny)cbdata.ptrval;

#if	DEBUG_DATABROWSER
        fprintf(stderr,"deleting %s\n", node->name);
#endif        
        
        UpdatePanes(go,_brHLUVAR,
                    NrmStringToQuark(node->name),NrmNULLQUARK,True,NULL);
        
	return;
}

static void
PlotDeleteCB
(
	NhlArgVal	cbdata,
	NhlArgVal	udata
)
{
	NgGO go = (NgGO) udata.ptrval;
        NgNclAny node = (NgNclAny)cbdata.ptrval;

#if	DEBUG_DATABROWSER
        fprintf(stderr,"deleting %s\n", node->name);
#endif        
        
        UpdatePanes(go,_brPLOTVAR,
                    NrmStringToQuark(node->name),NrmNULLQUARK,True,NULL);
        
	return;
}

static NgPageSaveState GetPageSaveState
(
	NgBrowse	browse,
        NrmQuark	qvar,
        NrmQuark	qfile
)
{
	NgBrowseClassRec *bclass = (NgBrowseClassRec *) 
		browse->base.layer_class;
	NgBrowseClassPart *bcp = &bclass->browse_class;
	NgPageSaveState ps_state;
	int i,count;

	if (! bcp->hidden_page_state)
		return NULL;

	count = XmLArrayGetCount(bcp->hidden_page_state);

	for (i = 0; i < count; i++) {
		ps_state = XmLArrayGet(bcp->hidden_page_state,i);
		if (ps_state->qvar == qvar && ps_state->qfile == qfile) {
			return ps_state;
		}
	}
	return NULL;
}

static brPage *AddPage
(
	NgGO		go,
        brPane		*pane,
        brPageType	type,
        NrmQuark	qvar,
        NrmQuark	qfile,
        brPage		*copy_page,
	NhlPointer	init_data
        )
{
	NgBrowse	browse = (NgBrowse)go;
	NgBrowsePart	*np = &browse->browse;
        char		*e_text;
        brPage		*page;
        int		pos;
        Dimension	tab_height;
        Dimension	w,h;
	NgPageSaveState ps_state = NULL;
      
        page = NhlMalloc(sizeof(brPage));
        if (!page) {
                e_text = "%s: dynamic memory allocation error";
                NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,_NhlName(go));
                return NULL;
        }
        page->go = go;
        page->type = type;
        page->qvar = qvar;
        page->qfile = qfile;
        page->tab = NULL;
        pos = InsertPage(pane,page);
        
        if (copy_page) /* assumes the 'copy_page' will be deleted */
                page->id = copy_page->id;
        else {
		ps_state = GetPageSaveState(browse,qvar,qfile);
		if (ps_state)
			page->id = ps_state->page_id;
		else
			page->id = ++CurrentPageId;
	}
        
#if	DEBUG_DATABROWSER & DEBUG_FOLDER
        fprintf(stderr,"AddPage: %x -- pane %d %s\n",
               page,PaneNumber(go,pane),PageString(pane,page,False));
#endif
        
        if (! pane->has_folder)
		CreateFolder(go,pane);

        XtVaGetValues(XtParent(pane->form),
                      XmNheight,&h,
                      XmNwidth,&w,
                      NULL);
#if DEBUG_DATABROWSER
        fprintf(stderr,"clip window w,h: %d,%d\n",w,h);
#endif
	/*
	 * since the page types are only pseudo-classes, there's no way to
	 * use function pointers before instance initialization, so we
	 * need this clumsy mechanism (maybe that will change one day).
	 */

        switch (type) {
        case _brREGVAR:
		page->pdata = _NgGetVarPage
			(go,pane,page,copy_page,ps_state,init_data);
		break;
        case _brFILEVAR:
		page->pdata = _NgGetVarPage
			(go,pane,page,copy_page,ps_state,init_data);
                break;
        case _brFILEREF:
		page->pdata = _NgGetFileRefPage
			(go,pane,page,copy_page,ps_state,init_data);
                break;
        case _brHLUVAR:
		page->pdata = _NgGetHluPage
			(go,pane,page,copy_page,ps_state,init_data);
                break;
        case _brHTML:
		page->pdata = _NgGetHtmlPage
			(go,pane,page,copy_page,ps_state,init_data);
                break;
        case _brPLOTVAR:
		page->pdata = _NgGetPlotPage
			(go,pane,page,copy_page,ps_state,init_data);
                break;
        }
        if (!page->pdata) {
                e_text = "%s: dynamic memory allocation error";
                NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,_NhlName(go));
                return NULL;
        }
        page->pdata->pane = pane;
	
        UpdateTabs(go,pane,pos,_ADD);
        
	(*page->pdata->adjust_page_geo)(page);

        XmScrollVisible(pane->scroller,page->tab->tab,9,9);
        
	(*page->pdata->adjust_page_geo)(page);

        return page;
}

/*
 * RemovePage disconnects a page from its current tab in a pane, in
 * preparation for moving it to another position (tab) in the same pane.
 * So this is not at all the same as 'DeletePage', which destroy the  
 * page as an entity.
 */

static brPage *
RemovePage
(
	NgGO		go,
        brPane		*pane,
        int		pos
        )
{
        brPage	*page = XmLArrayGet(pane->pagelist,pos);

        NgReleasePageHtmlViews(go->base.id,page->id);
        XmLArrayDel(pane->pagelist,pos,1);
        pane->pagecount--;

        return page;
}

static void DeletePage(
	NgGO		go,
        brPane		*pane,
        brPage		*page
        )
{
        int i;
        
        if (pane->active_page == page)
                pane->active_page = NULL;

	if (pane->pagecount == 0) {
#if	DEBUG_DATABROWSER & DEBUG_FOLDER
                fprintf(stderr,"DeletePage: %x -- pane %d %s\n",
                       page,PaneNumber(go,pane),PageString(pane,page,False));
#endif
		if (page->pdata->deactivate_page)
			(*page->pdata->deactivate_page)(page);
                page->pdata->in_use = False;
                NclFreeDataList(page->pdata->dl);
                page->pdata->dl = NULL;
                NhlFree(page);
		DestroyFolder(go,pane);
		return;
	}

	UpdateTabs(go,pane,-1,_REMOVE);

#if	DEBUG_DATABROWSER & DEBUG_FOLDER
        fprintf(stderr,"DeletePage: %x -- pane %d %s\n",
               page,PaneNumber(go,pane),PageString(pane,page,False));
#endif
        if (page->pdata->deactivate_page)
                (*page->pdata->deactivate_page)(page);
        page->pdata->in_use = False;
        NclFreeDataList(page->pdata->dl);
        page->pdata->dl = NULL;

        NhlFree(page);
        
        return;
}
        
        
static void
ShufflePage
(
	NgGO		go,
        brPane		*pane,
        int		pos
        )
{
        brPage	*page;
        int	newpos;

        if (pane->pagecount <= 1)
                return;
        
        page = RemovePage(go,pane,pos);

        newpos = InsertPage(pane,page);

        UpdateTabs(go,pane,newpos,_SHUFFLE);
        
        XmScrollVisible(pane->scroller,page->tab->tab,9,9);

	(*page->pdata->adjust_page_geo)(page);

        return;
}

static brPage *
UpdatePanes
(
	NgGO		go,
        brPageType	type,
        NrmQuark	qvar,
        NrmQuark	qfile,
	NhlBoolean	delete,
	NhlPointer	init_data
)
{
	NgBrowse	browse = (NgBrowse)go;
	NgBrowsePart	*np = &browse->browse;
        brPaneControl	*pcp = &np->pane_ctrl;
        NhlBoolean	page_found = False;
        int 		i,j,pos = -1;
        brPane		*delete_pane = NULL;
        brPage		*page = NULL,*copy_page = NULL;

        for (i = 0; i < pcp->alloc_count; i++) {
                brPane	*this_pane = pcp->panes[i];

                for (j = 0; j < this_pane->pagecount; j++) {
                        page = XmLArrayGet(this_pane->pagelist,j);
                        this_pane->remove_pos = -1;
			if (page->qvar == qvar && page->qfile == qfile) {
                                if (! delete && this_pane == CurrentPane(go)) {
                                        page_found = True;
                                        ShufflePage(go,this_pane,j);
                                }
                                else {
                                        this_pane->remove_pos = j;
                                        delete_pane = this_pane;
                                        copy_page = page;
                                }
                                break;
                        }
                }
        }
/*
 * In order for functions that depend on getting the pane based on the
 * page id to work, it is necessary to update the page lists before the page is
 * actually created. The page will be added to its new pane in the AddPage
 * function, before the NgGet...Page call. Note that a copied page cannot 
 * be deleted until the new page is created, so that the data structures can
 * be copied.
 */
        if (delete_pane)
                RemovePage(go,delete_pane,delete_pane->remove_pos);
        
	if (! delete) {
		if (! page_found) {
                	page = AddPage
                                (go,CurrentPane(go),
				 type,qvar,qfile,copy_page,init_data);
#if 0
			XSync(go->go.x->dpy,False);
#endif
        	}
                if (delete_pane)
                        DeletePage(go,delete_pane,copy_page);
                SetTabFocus(go,CurrentPane(go),page);
        	NextPane(go);
	}
        else if (delete_pane)
                DeletePage(go,delete_pane,copy_page);
        
        return page;
}

typedef struct _timer_data 
{
        NgGO go;
        NrmQuark qvar;
        brPageType type;
} timer_data;

static void BrowseTimeoutCB 
(
	XtPointer	data,
        XtIntervalId	*timer
        )
{
        timer_data	*tdata = (timer_data *)data;
	NgGO		go = (NgGO) tdata->go;
	NgBrowse	browse = (NgBrowse)go;
	NgBrowsePart	*np = &browse->browse;
	NrmQuark	qvar = tdata->qvar;

        switch (tdata->type) {
            case _brREGVAR:
                    UpdatePanes(go,_brREGVAR,qvar,NrmNULLQUARK,False,NULL);
                    break;
            case _brFILEREF:
                    UpdatePanes(go,_brFILEREF,NrmNULLQUARK,qvar,False,NULL);
                    break;
            case _brFILEVAR:
                    UpdatePanes(go,_brFILEVAR,
                                qvar,np->vmenus->qfile,False,NULL);
                    break;
            case _brHLUVAR:
                    UpdatePanes(go,_brHLUVAR,qvar,NrmNULLQUARK,False,NULL);
                    break;
            case _brHTML:
                    UpdatePanes(go,_brHTML,qvar,NrmNULLQUARK,False,NULL);
                    break;
            case _brPLOTVAR:
                    UpdatePanes(go,_brPLOTVAR,qvar,NrmNULLQUARK,False,NULL);
                    break;
        }
	XSync(go->go.x->dpy,False);
        return;

}
        
static void BrowseHluCB 
(
	Widget		w,
	XtPointer	udata,
	XtPointer	cb_data
)
{
	NgGO		go = (NgGO) udata;
	NgBrowse	browse = (NgBrowse)udata;
	NgBrowsePart	*np = &browse->browse;
	NrmQuark	qvar;
        brPage		*page;
        brPane		*pane;
        static timer_data tdata;
        
#if	DEBUG_DATABROWSER & DEBUG_ENTRY
	fprintf(stderr,"BrowseVarCB(IN)\n");
#endif
	XtVaGetValues(w,
		      XmNuserData,&qvar,
		      NULL);

#if	DEBUG_DATABROWSER & DEBUG_FOLDER
	fprintf(stderr,"browsing var %s\n", NrmQuarkToString(qvar));
#endif

        tdata.go = go;
        tdata.qvar = qvar;
        tdata.type = _brHLUVAR;
        
        XtAppAddTimeOut(go->go.x->app,50,BrowseTimeoutCB,&tdata);
        
	return;
}

        
static void BrowseVarCB 
(
	Widget		w,
	XtPointer	udata,
	XtPointer	cb_data
)
{
	NgGO		go = (NgGO) udata;
	NgBrowse	browse = (NgBrowse)udata;
	NgBrowsePart	*np = &browse->browse;
	NrmQuark	qvar;
        brPage		*page;
        brPane		*pane;
        static timer_data tdata;
        
#if	DEBUG_DATABROWSER & DEBUG_ENTRY
	fprintf(stderr,"BrowseVarCB(IN)\n");
#endif
	XtVaGetValues(w,
		      XmNuserData,&qvar,
		      NULL);

#if	DEBUG_DATABROWSER & DEBUG_FOLDER
	fprintf(stderr,"browsing var %s\n", NrmQuarkToString(qvar));
#endif

        tdata.go = go;
        tdata.qvar = qvar;
        tdata.type = _brREGVAR;
        
        XtAppAddTimeOut(go->go.x->app,50,BrowseTimeoutCB,&tdata);
        
	return;
}


static void BrowseFileCB 
(
	Widget		w,
	XtPointer	udata,
	XtPointer	cb_data
)
{
	NgGO		go = (NgGO) udata;
	NgBrowse	browse = (NgBrowse)udata;
	NgBrowsePart	*np = &browse->browse;
	NrmQuark	qfile;
        brPage		*page;
        brPane		*pane;
        static timer_data tdata;

#if	DEBUG_DATABROWSER & DEBUG_ENTRY
	fprintf(stderr,"BrowseFileCB(IN)\n");
#endif
	XtVaGetValues(w,
		      XmNuserData,&qfile,
		      NULL);

#if	DEBUG_DATABROWSER & DEBUG_FOLDER
	fprintf(stderr,"browsing file %s\n", NrmQuarkToString(qfile));
#endif
        
        tdata.go = go;
        tdata.qvar = qfile;
        tdata.type = _brFILEREF;
        
        XtAppAddTimeOut(go->go.x->app,50,BrowseTimeoutCB,&tdata);

	return;
}

static void BrowseFileVarCB 
(
	Widget		w,
	XtPointer	udata,
	XtPointer	cb_data
)
{
	NgGO		go = (NgGO) udata;
	NgBrowse	browse = (NgBrowse)go;
	NgBrowsePart	*np = &browse->browse;
	NrmQuark	qvar;
        brPage		*page;
        brPane		*pane;
        static timer_data tdata;

#if	DEBUG_DATABROWSER & DEBUG_ENTRY
	fprintf(stderr,"BrowseFileVarCB(IN)\n");
#endif
	XtVaGetValues(w,
		      XmNuserData,&qvar,
		      NULL);
#if	DEBUG_DATABROWSER & DEBUG_FOLDER
	fprintf(stderr,"browsing filevar %s in file %s\n",
               NrmQuarkToString(qvar),
               NrmQuarkToString(np->vmenus->qfile));
#endif

        tdata.go = go;
        tdata.qvar = qvar;
        tdata.type = _brFILEVAR;
        
        XtAppAddTimeOut(go->go.x->app,50,BrowseTimeoutCB,&tdata);
        
	return;
}
        
static void BrowseFunctionCB 
(
	Widget		w,
	XtPointer	udata,
	XtPointer	cb_data
)
{
	NgGO		go = (NgGO) udata;
	NgBrowse	browse = (NgBrowse)udata;
	NgBrowsePart	*np = &browse->browse;
	NrmQuark	qvar;
        brPage		*page;
        brPane		*pane;
        static timer_data tdata;
        
#if	DEBUG_DATABROWSER & DEBUG_ENTRY
	fprintf(stderr,"BrowseFunctionCB(IN)\n");
#endif
	XtVaGetValues(w,
		      XmNuserData,&qvar,
		      NULL);

#if	DEBUG_DATABROWSER & DEBUG_FOLDER
	fprintf(stderr,"browsing function %s\n", NrmQuarkToString(qvar));
#endif

        tdata.go = go;
        tdata.qvar = qvar;
        tdata.type = _brFUNCTION;
        
        XtAppAddTimeOut(go->go.x->app,50,BrowseTimeoutCB,&tdata);
        
	return;
}

        
static void BrowsePlotCB 
(
	Widget		w,
	XtPointer	udata,
	XtPointer	cb_data
)
{
	NgGO		go = (NgGO) udata;
	NgBrowse	browse = (NgBrowse)udata;
	NgBrowsePart	*np = &browse->browse;
	NrmQuark	qvar;
        brPage		*page;
        brPane		*pane;
        static timer_data tdata;
        
#if	DEBUG_DATABROWSER & DEBUG_ENTRY
	fprintf(stderr,"BrowseVarCB(IN)\n");
#endif
	XtVaGetValues(w,
		      XmNuserData,&qvar,
		      NULL);

#if	DEBUG_DATABROWSER & DEBUG_FOLDER
	fprintf(stderr,"browsing var %s\n", NrmQuarkToString(qvar));
#endif

        tdata.go = go;
        tdata.qvar = qvar;
        tdata.type = _brPLOTVAR;
        
        XtAppAddTimeOut(go->go.x->app,50,BrowseTimeoutCB,&tdata);
        
	return;
}

static NgVarMenus
CreateVarMenus
(
	NgGO	go,
        Widget	parent
)
{
	NgBrowse	browse = (NgBrowse)go;
	NgBrowsePart	*np = &browse->browse;
        Widget		menubar,label;

        label = XtVaCreateManagedWidget
                ("Browse:",xmLabelGadgetClass,
                 parent,
                 XmNrightAttachment,	XmATTACH_NONE,
                 NULL);
        
	menubar = XtVaCreateManagedWidget
                ("BrowseMenu",xmRowColumnWidgetClass,
                 parent,
                 XmNrowColumnType,	XmMENU_BAR,
                 XmNleftAttachment,	XmATTACH_WIDGET,
                 XmNleftWidget,		label,
                 XmNleftOffset,		15,
                 XmNrightAttachment,	XmATTACH_NONE,
                 NULL);
        return NgCreateVarMenus(menubar,
				BrowseHluCB,
                                BrowseVarCB,
                                BrowseFileCB,
                                BrowseFileVarCB,
#if 0
				BrowseFunctionCB,
#endif
				NULL,
                                browse);
}
static void
AdjustPaneGeometry
(
	NgGO	go
	)
{
	NgBrowse	browse = (NgBrowse)go;
	NgBrowsePart	*np = &browse->browse;
        brPaneControl	*pcp = &np->pane_ctrl;
	int i;

#if DEBUG_DATABROWSER
	fprintf(stderr,"in adjust pane geometry\n");
#endif

	for (i = 0; i < pcp->current_count; i++) {
		brPane	*pane = pcp->panes[i];
		brPage	*page;


		if (! pane->has_folder || ! pane->pagelist)
			continue;
		page = XmLArrayGet(pane->pagelist,pane->active_pos);
		if (!page) {
#if DEBUG_DATABROWSER
			fprintf(stderr,"error retrieving active page\n");
#endif
			return;
		}
		(*page->pdata->adjust_page_geo)(page);
	}
	return;
}

static void PaneCtrlCB 
(
	Widget		w,
	XtPointer	udata,
	XtPointer	cb_data
)
{
	NgGO		go = (NgGO) udata;
	NgBrowse	browse = (NgBrowse)go;
	NgBrowsePart	*np = &browse->browse;
        brPaneControl	*pcp = &np->pane_ctrl;
        char		buf[10];
        
#if	DEBUG_DATABROWSER & DEBUG_ENTRY
	fprintf(stderr,"PaneCtrlCB(IN)\n");
#endif
        
        if (w == pcp->vcr->reverse) {
                RemovePane(go);
        }
        else if (w == pcp->vcr->forward) {
                if (pcp->current_count == brMAX_PANES)
                        return;
                AddPane(go);
        }

        sprintf(buf,"%d",pcp->current_count);
        XtVaSetValues(pcp->text,
                      XmNvalue,buf,
                      NULL);

#if	DEBUG_DATABROWSER & DEBUG_FOLDER
	fprintf(stderr,"in_use %d current_ix %d alloc %d\n",
                pcp->current_count,pcp->current_ix,pcp->alloc_count);
#endif
        AdjustPaneGeometry(go);
        
	return;
}

static void DeleteSelectionCB 
(
	Widget		w,
	XtPointer	udata,
	XtPointer	cb_data
)
{
	NgGO		go = (NgGO) udata;
	NgBrowse	browse = (NgBrowse)go;
	NgBrowsePart	*np = &browse->browse;
        brPaneControl	*pcp = &np->pane_ctrl;
        brPage		*page;
        brPane		*pane;
        NrmQuark	qvar,qfile;
        int		i;
        
#if	DEBUG_DATABROWSER & DEBUG_ENTRY
	fprintf(stderr,"CycleSelectionCB(IN)\n");
#endif
        if (! pcp->focus_pane || pcp->focus_pos < 0
            || pcp->focus_pos >= pcp->focus_pane->pagecount)
                return;
        page = XmLArrayGet(pcp->focus_pane->pagelist,pcp->focus_pos);

	DeleteVarPage(page);
        
        return;
}

static void CycleSelectionCB 
(
	Widget		w,
	XtPointer	udata,
	XtPointer	cb_data
)
{
	NgGO		go = (NgGO) udata;
	NgBrowse	browse = (NgBrowse)go;
	NgBrowsePart	*np = &browse->browse;
        brPaneControl	*pcp = &np->pane_ctrl;
        brPage		*page;
        brPane		*pane;
        NrmQuark	qvar,qfile;
        int		i;
        
#if	DEBUG_DATABROWSER & DEBUG_ENTRY
	fprintf(stderr,"CycleSelectionCB(IN)\n");
#endif
        if (! pcp->focus_pane || pcp->focus_pos < 0
            || pcp->focus_pos >= pcp->focus_pane->pagecount)
                return;
        page = XmLArrayGet(pcp->focus_pane->pagelist,pcp->focus_pos);
        qvar = page->qvar;
        qfile = page->qfile;

        for (i = 0; i < pcp->current_count; i++) {
                if (pcp->panes[i] == pcp->focus_pane) {
                        pcp->current_ix = i;
                        break;
                }
        }
        NextPane(go);

	UpdatePanes(go,page->type,page->qvar,page->qfile,False,NULL);
        
        return;
	
}

static void UpdatePagesCB 
(
	Widget		w,
	XtPointer	udata,
	XtPointer	cb_data
)
{
	NgGO		go = (NgGO) udata;
	NgBrowse	browse = (NgBrowse)go;
	NgBrowsePart	*np = &browse->browse;
        brPaneControl	*pcp = &np->pane_ctrl;
        brPage		*page;
        brPane		*pane;
        NrmQuark	qvar,qfile;
        int		i,j;
	NgWksState 	wks_state;
        
	NhlVAGetValues(go->go.appmgr,
		       NgNappWksState,&wks_state,
		       NULL);

#if	DEBUG_DATABROWSER & DEBUG_ENTRY
	fprintf(stderr,"UpdatePagesCB(IN)\n");
#endif

        for (i = 0; i < pcp->current_count; i++) {
		pane = np->pane_ctrl.panes[i];
                for (j = 0; j < pane->pagecount; j++) {
                        brPage *page = (brPage *)XmLArrayGet(pane->pagelist,j);
			if (! page->pdata->update_page)
				continue;
			(*page->pdata->update_page)(page);
                }
        }

	NgDrawUpdatedViews(wks_state,False);
	
        return;
	
}

static void HelpCB 
(
	Widget		w,
	XtPointer	udata,
	XtPointer	cb_data
)
{
	NgGO		go = (NgGO) udata;
	NgBrowse	browse = (NgBrowse)go;
	NgBrowsePart	*np = &browse->browse;
	NrmQuark	qhtml;

	qhtml = NrmStringToQuark("Start.html");
	NgOpenPage(go->base.id,_brHTML,&qhtml,1,NULL);
	
}
static void
ChangeSizeEH
(
	Widget		w,
	XtPointer	udata,
	XEvent		*event,
	Boolean		*cont
)
{
        NgGO go		= (NgGO) udata;
	NgBrowse	browse = (NgBrowse)go;
	NgBrowsePart	*np = &browse->browse;
        brPaneControl	*pcp = &np->pane_ctrl;
	NrmQuark	qhtml;
        static timer_data tdata;

#if	DEBUG_DATABROWSER & DEBUG_FOLDER
        fprintf(stderr,"EH height %d width %d\n",
                event->xconfigure.height,
                event->xconfigure.width);
#endif
        
	if(event->type == ConfigureNotify) {
#if	DEBUG_DATABROWSER & DEBUG_FOLDER
                fprintf(stderr,"ConfigureNotify\n");
#endif
		if (! np->mapped)
			return;
        }
        else if (event->type == MapNotify) {
#if	DEBUG_DATABROWSER & DEBUG_FOLDER
                fprintf(stderr,"MapNotify\n");
#endif
		if (! np->mapped) {
			np->mapped = True;

			tdata.go = go;
			tdata.qvar = qhtml = NrmStringToQuark("Start.html");
			tdata.type = _brHTML;
			XtAppAddTimeOut(go->go.x->app,200,
					BrowseTimeoutCB,&tdata);
		}
        }
	else {
		return;
	}


	AdjustPaneGeometry(go);

	return;
}

static void
SetupPaneControl
(
	NgGO	go,
        Widget	parent
)
{
	NgBrowse	browse = (NgBrowse)go;
	NgBrowsePart	*np = &browse->browse;
        Widget		label,pb;
        brPaneControl	*pcp = &np->pane_ctrl;
        Pixel		foreground,background;

        label = XtVaCreateManagedWidget
                ("Panes:",xmLabelGadgetClass,
                 parent,
                 XmNrightAttachment,	XmATTACH_NONE,
                 NULL);
        
        pcp->alloc_count = 0;
        pcp->current_count = 0;
        pcp->current_ix = 0;
        pcp->focus_pane = NULL;
        pcp->focus_pos = 0;

        pcp->text = XtVaCreateManagedWidget
                ("PaneCount",xmTextFieldWidgetClass,
                 parent,
                 XmNleftAttachment,	XmATTACH_WIDGET,
                 XmNleftWidget,		label,
                 XmNleftOffset,		15,
                 XmNrightAttachment,	XmATTACH_NONE,
                 XmNbottomAttachment,	XmATTACH_NONE,
                 XmNvalue,		"1",
                 XmNcolumns,		2,
                 XmNeditable,		False,
                 XmNresizeWidth,	True,
                 XmNtopOffset,		5,
                 XmNbottomOffset,	5,
                 NULL);
        
        pcp->vcr = NgCreateVcrControl
                (go,"PaneInc",parent,20,
                 False,False,False,True,False,True,False,False);

        XtVaSetValues
                (pcp->vcr->form,
                 XmNleftAttachment,	XmATTACH_WIDGET,
                 XmNleftWidget,		pcp->text,
                 XmNbottomAttachment,	XmATTACH_OPPOSITE_WIDGET,
                 XmNbottomWidget,	pcp->text,
                 XmNtopAttachment,	XmATTACH_OPPOSITE_WIDGET,
                 XmNtopWidget,		pcp->text,
                 XmNrightAttachment,	XmATTACH_NONE,
                 NULL);
        
        XtAddCallback(pcp->vcr->reverse,XmNactivateCallback,PaneCtrlCB,go);
        XtAddCallback(pcp->vcr->forward,XmNactivateCallback,PaneCtrlCB,go);

        label = XtVaCreateManagedWidget
                ("Pages:",xmLabelGadgetClass,
                 parent,
                 XmNleftAttachment,	XmATTACH_WIDGET,
                 XmNleftWidget,		pcp->vcr->form,
		 XmNleftOffset,		15,
                 XmNrightAttachment,	XmATTACH_NONE,
                 NULL);

        pb = XtVaCreateManagedWidget
                ("Cycle",xmPushButtonWidgetClass,
                 parent,
                 XmNleftOffset,		15,
                 XmNleftAttachment,	XmATTACH_WIDGET,
                 XmNleftWidget,		label,
                 XmNrightAttachment,	XmATTACH_NONE,
                 XmNbottomAttachment,	XmATTACH_NONE,
                 XmNtopOffset,		5,
                 XmNbottomOffset,	5,
                 NULL);
        XtAddCallback(pb,XmNactivateCallback,CycleSelectionCB,go);


        pb = XtVaCreateManagedWidget
                ("Hide",xmPushButtonWidgetClass,
                 parent,
                 XmNleftOffset,		15,
                 XmNleftAttachment,	XmATTACH_WIDGET,
                 XmNleftWidget,		pb,
                 XmNrightAttachment,	XmATTACH_NONE,
                 XmNbottomAttachment,	XmATTACH_NONE,
                 XmNtopOffset,		5,
                 XmNbottomOffset,	5,
                 NULL);
        XtAddCallback(pb,XmNactivateCallback,DeleteSelectionCB,go);

        pb = XtVaCreateManagedWidget
                ("Update",xmPushButtonWidgetClass,
                 parent,
                 XmNleftOffset,		15,
                 XmNleftAttachment,	XmATTACH_WIDGET,
                 XmNleftWidget,		pb,
                 XmNrightAttachment,	XmATTACH_NONE,
                 XmNbottomAttachment,	XmATTACH_NONE,
                 XmNtopOffset,		5,
                 XmNbottomOffset,	5,
                 NULL);
        XtAddCallback(pb,XmNactivateCallback,UpdatePagesCB,go);

        return;
}

static NhlBoolean
BrowseCreateWin
(
	NgGO	go
)
{
	char		func[]="BrowseCreateWin";
	NgBrowse	browse = (NgBrowse)go;
	NgBrowsePart	*np = &browse->browse;
        Widget		datamenubar;
	Widget		w,form,sep;
        brPane		*pane;
        XmString	xmstring;
	NhlArgVal	sel,user_data;
	NhlLayer	ncl = _NhlGetLayer(np->nsid);
                
	np->mapped = False;

	XtVaSetValues(go->go.shell,
		      XmNkeyboardFocusPolicy,XmEXPLICIT,
		      NULL);
	XtAppAddActions(go->go.x->app,
                        browseactions,NhlNumber(browseactions));

        XtAddEventHandler(go->go.manager,StructureNotifyMask,
                          False,ChangeSizeEH,(XtPointer)go);

	_NgGOSetTitle(go,"NCAR DataVision Browser",NULL);
	_NgGOCreateMenubar(go);

	XtVaSetValues(go->go.close,
		XmNsensitive,	False,
		NULL);

	XtVaSetValues(go->go.help,
		XmNsensitive,	True,
		NULL);

	w = XtVaCreateManagedWidget("helpBtn",xmPushButtonGadgetClass,
		go->go.hmenu,NULL);
	XtAddCallback(w,XmNactivateCallback,HelpCB,go);
	

        form = XtVaCreateManagedWidget
                ("form",xmFormWidgetClass,
                 go->go.manager,
                 XmNtopAttachment,	XmATTACH_WIDGET,
                 XmNtopWidget,		go->go.menubar,
                 XmNbottomAttachment,	XmATTACH_NONE,
                 NULL);
        np->vmenus = CreateVarMenus(go,form);

        form = XtVaCreateManagedWidget
                ("form",xmFormWidgetClass,
                 go->go.manager,
                 XmNtopAttachment,	XmATTACH_WIDGET,
                 XmNtopWidget,		form,
                 XmNbottomAttachment,	XmATTACH_NONE,
                 NULL);
        SetupPaneControl(go,form);
        
        sep = XtVaCreateManagedWidget
                ("sep",xmSeparatorGadgetClass,
                 go->go.manager,
                 XmNtopAttachment,	XmATTACH_WIDGET,
                 XmNtopWidget,	form,
                 XmNbottomAttachment,	XmATTACH_NONE,
                 NULL);

	np->paned_window = XtVaCreateManagedWidget
                ("paned_window",xmPanedWindowWidgetClass,
                 go->go.manager,
                 XmNallowResize,	True,
                 XmNtopAttachment,	XmATTACH_WIDGET,
                 XmNtopWidget,		sep,
                 NULL);


        pane = AddPane(go);
        
	NhlINITVAR(sel);
	NhlINITVAR(user_data);
	user_data.ptrval = go;
        sel.lngval = NgNclCBDELETE_VAR;
	_NhlAddObjCallback(ncl,NgCBnsObject,sel,VarDeleteCB,user_data);
        sel.lngval = NgNclCBDELETE_FILEVAR;
	_NhlAddObjCallback(ncl,NgCBnsObject,sel,FileRefDeleteCB,user_data);
        sel.lngval = NgNclCBDELETE_HLUVAR;
	_NhlAddObjCallback(ncl,NgCBnsObject,sel,HluVarDeleteCB,user_data);

	return True;
}

extern void _NgGetPaneVisibleArea(
        NhlLayer go,
        brPane *pane,
        XRectangle *rect
        )
{
        Position form_x, form_y, clip_x, clip_y;
        Dimension clip_width, clip_height;
	Dimension vsb_width = 0, hsb_height = 0;
        
        XtVaGetValues(pane->form,
                      XmNx,&form_x,
                      XmNy,&form_y,
                      NULL);
		
        XtVaGetValues(pane->clip_window,
                      XmNx,&clip_x,
                      XmNy,&clip_y,
                      XmNwidth,&clip_width,
                      XmNheight,&clip_height,
                      NULL);
        rect->x = clip_x - form_x;
        rect->y = clip_y - form_y;
        rect->width = clip_width;
        rect->height = clip_height;

        return;
}

extern brPane *_NgGetPaneOfPage(
        int		goid,
        NgPageId	page_id
        )
{
        char func[] = "_NgGetPaneOfPage";
        NhlLayer go = _NhlGetLayer(goid);
	NgBrowse	browse;
	NgBrowsePart	*np;
        brPane *pane;
        int i,j;
        
        if (! go) {
                NHLPERROR((NhlFATAL,NhlEUNKNOWN,"%s: invalid layer id",
                           func));
                return NULL;
        }
        browse  = (NgBrowse)go;
        np = &browse->browse;

            /* find the pane */

        for (i = 0; i < np->pane_ctrl.current_count; i++) {
                pane = np->pane_ctrl.panes[i];
                for (j = 0; j < pane->pagecount; j++) {
                        brPage *page = (brPage *)XmLArrayGet(pane->pagelist,j);
                        if (page->id == page_id)
                                return pane;
                }
        }
        return NULL;
}

extern brPage *_NgGetPageRef(
        int		goid,
        NgPageId	page_id
        )
{
        char func[] = "_NgGetPageRef";
        NhlLayer go = _NhlGetLayer(goid);
	NgBrowse	browse;
	NgBrowsePart	*np;
        brPane *pane;
        int i,j;
        
        if (! go) {
                NHLPERROR((NhlFATAL,NhlEUNKNOWN,"%s: invalid layer id",
                           func));
                return NULL;
        }
        browse  = (NgBrowse)go;
        np = &browse->browse;

            /* find the pane */

        for (i = 0; i < np->pane_ctrl.current_count; i++) {
                pane = np->pane_ctrl.panes[i];
                for (j = 0; j < pane->pagecount; j++) {
                        brPage *page = (brPage *)XmLArrayGet(pane->pagelist,j);
                        if (page->id == page_id)
                                return page;
                }
        }
        return NULL;
}
        
extern NgPageId NgOpenPage(
        int		goid,
        brPageType	type,
        NrmQuark	*qname,
        int		qcount,
	NhlPointer	init_data
        )
{
        NgGO		go = (NgGO)_NhlGetLayer(goid);
        brPage		*page;
        
        switch (type) {
            case _brREGVAR:
                    if (qcount < 1 || qname[0] == NrmNULLQUARK)
                            return NgNoPage;
                    page = UpdatePanes(go,_brREGVAR,
                                       qname[0],NrmNULLQUARK,False,init_data);
                    break;
            case _brFILEREF:
                    if (qcount < 1 || qname[0] == NrmNULLQUARK)
                            return NgNoPage;
                    page = UpdatePanes(go,_brFILEREF,
                                       NrmNULLQUARK,qname[0],False,init_data);
                    break;
            case _brFILEVAR:
                    if (qcount < 2
                        || qname[0] == NrmNULLQUARK
                        || qname[1] == NrmNULLQUARK)
                            return NgNoPage;
                    page = UpdatePanes
			    (go,_brFILEVAR,qname[0],qname[1],False,init_data);
                    break;
            case _brHLUVAR:
                    if (qcount < 1 || qname[0] == NrmNULLQUARK)
                            return NgNoPage;
                    page = UpdatePanes(go,_brHLUVAR,
                                       qname[0],NrmNULLQUARK,False,init_data);
                    break;
            case _brHTML:
                    page = UpdatePanes(go,_brHTML,
                                       qname[0],NrmNULLQUARK,False,init_data);
                    break;
            case _brPLOTVAR:
                    if (qcount < 1 || qname[0] == NrmNULLQUARK)
                            return NgNoPage;
                    page = UpdatePanes(go,_brPLOTVAR,
                                       qname[0],NrmNULLQUARK,False,init_data);
                    break;
        }
        return page ? page->id : NgNoPage;

}

static brPage *GetPageReference
(
        NgGO	go,
        NgPageId	id
        )
{
	NgBrowse	browse = (NgBrowse)go;
	NgBrowsePart	*np = &browse->browse;
        brPaneControl	*pcp = &np->pane_ctrl;
        brPane		*pane;
        brPage		*page;
        int		i,j;
         
        for (i = 0; i < pcp->alloc_count; i++) {
                pane = pcp->panes[i];
                for (j = 0; j < pane->pagecount; j++) {
                        page = XmLArrayGet(pane->pagelist,j);
                        if (page->id == id)
                                return page;
                }
        }
        return NULL;
}
        
extern NhlPointer NgPageData(
        int		goid,
        NgPageId	page_id
        )
{
        NgGO		go = (NgGO)_NhlGetLayer(goid);
        brPage		*page;

	if (! go)
		return NULL;
	page = GetPageReference(go,page_id);

        if (! page)
                return NULL;
        if (! page->pdata->public_page_data)
                return NULL;

        return ((*page->pdata->public_page_data)(page));

}

extern NhlErrorTypes NgResetPage
(
        int		goid,
        NgPageId	page_id
        )        
{
        NgGO		go = (NgGO)_NhlGetLayer(goid);
        brPage		*page = GetPageReference(go,page_id);

        if (! page)
                return NhlFATAL;
        if (! page->pdata->reset_page)
                return NhlFATAL;

        return ((*page->pdata->reset_page)(page));

}

/*
 * the force_draw parameter forces all visible graphics to be redrawn.
 * it does not override the 'draw single view' option.
 */
extern NhlErrorTypes NgUpdatePages
(
        int		goid,
	NhlBoolean	force_draw
        )        
{
	NgGO		go = (NgGO) _NhlGetLayer(goid);
	NgBrowse	browse = (NgBrowse)go;
	NgBrowsePart	*np;
        brPaneControl	*pcp;
        brPage		*page;
        brPane		*pane;
        NrmQuark	qvar,qfile;
        int		i,j;
	NgWksState 	wks_state;
         
	if (! browse) {
                NHLPERROR((NhlFATAL,NhlEUNKNOWN,"%s: invalid layer id",
                           "NgUpdatePages"));
                return NhlFATAL;
	}
	np = &browse->browse;
	pcp = &np->pane_ctrl;
		
	NhlVAGetValues(go->go.appmgr,
		       NgNappWksState,&wks_state,
		       NULL);

#if	DEBUG_DATABROWSER & DEBUG_ENTRY
	fprintf(stderr,"UpdatePagesCB(IN)\n");
#endif

        for (i = 0; i < pcp->current_count; i++) {
		pane = np->pane_ctrl.panes[i];
                for (j = 0; j < pane->pagecount; j++) {
                        brPage *page = (brPage *)XmLArrayGet(pane->pagelist,j);
			if (! page->pdata->update_page)
				continue;
			(*page->pdata->update_page)(page);
                }
        }

	NgDrawUpdatedViews(wks_state,force_draw);
	
        return NhlNOERROR;
	
}


extern NhlErrorTypes NgUpdatePage
(
        int		goid,
        NgPageId	page_id
        )        
{
        NgGO		go = (NgGO)_NhlGetLayer(goid);
        brPage		*page = GetPageReference(go,page_id);

        if (! page)
                return NhlFATAL;
        if (! page->pdata->update_page)
                return NhlFATAL;

        return ((*page->pdata->update_page)(page));

}

extern int
NgGetPageId
(
        int		goid,
        NrmQuark	qsym1,
        NrmQuark	qsym2
        )
{
        NgGO		go = (NgGO)_NhlGetLayer(goid);
	NgBrowse	browse = (NgBrowse)go;
	NgBrowsePart	*np = &browse->browse;
        brPaneControl	*pcp = &np->pane_ctrl;
        brPane		*pane;
        brPage		*page;
        int		i,j;
         
        for (i = 0; i < pcp->alloc_count; i++) {
                pane = pcp->panes[i];
                for (j = 0; j < pane->pagecount; j++) {
                        page = XmLArrayGet(pane->pagelist,j);
                        if (qsym2 > NrmNULLQUARK) {
                                if (page->qvar == qsym1 &&
                                    page->qfile == qsym2) {
                                        return page->id;
                                }
                        }
                        else if (page->qvar == qsym1) {
                                return page->id;
                        }
                }
        }
        return NgNoPage;
}

extern NhlErrorTypes NgPageSetVisible(
        int		goid,
        NgPageId	page_id,
        Widget		requestor,
        XRectangle	*rect
        )
{
        NgGO		go = (NgGO)_NhlGetLayer(goid);
        brPage		*page = GetPageReference(go,page_id);
        brPane 		*pane;
        XRectangle	pvis_rect,rvis_rect;
        Position	pane_x,pane_y,req_x,req_y;
        Dimension	pane_height,pane_width;
        int		min,max,ssize,value,inc,page_inc;

        if (! page) {
                NHLPERROR((NhlWARNING,NhlEUNKNOWN,"invalid page specified"));
                return NhlWARNING;
        }
                
        pane = page->pdata->pane;
        if (! pane->form)
                return NhlNOERROR;
/*
 * This routine adjusts the scrollbars to make as much of the specified
 * rectangle visible as possible. The rectangle is specified relative to
 * the "requestor" widget.
 */
        
	_NgGetPaneVisibleArea((NhlLayer) go,pane,&pvis_rect);
        
        XtTranslateCoords(pane->form,(Position) 0,(Position) 0,
                          &pane_x,&pane_y);
        XtTranslateCoords(requestor,(Position) 0,(Position) 0,
                          &req_x,&req_y);

        rvis_rect.x = req_x - pane_x + rect->x;
        rvis_rect.y = req_y - pane_y + rect->y;
        rvis_rect.height = rect->height;
        rvis_rect.width = rect->width;

        XtVaGetValues(pane->form,
                      XmNheight,&pane_height,
                      XmNwidth,&pane_width,
                      NULL);

        XtVaGetValues(pane->vsb,
                      XmNminimum,&min,
                      XmNmaximum,&max,
                      XmNsliderSize,&ssize,
                      XmNvalue,&value,
                      XmNincrement,&inc,
                      XmNpageIncrement,&page_inc,
                      NULL);

#if	DEBUG_DEBUG_DATABROWSER
        fprintf(stderr,
		"min %d max %d ssize %d value %d\n",min,max,ssize,value);
#endif
        if (rvis_rect.height > pvis_rect.height ||
            rvis_rect.y < pvis_rect.y) {
                value = min + ((float)rvis_rect.y)/pane_height *
                        (max - min);
                value = MAX(min,MIN(value,max-ssize));
                XmScrollBarSetValues
                        (pane->vsb,value,ssize,inc,page_inc,True);
        }
        else if (rvis_rect.y + rvis_rect.height  > 
                 pvis_rect.y + pvis_rect.height) {
                value = (((float)rvis_rect.y + rvis_rect.height) / pane_height)
                        * (max - min) + min - ssize;
                value = MAX(min,MIN(value,max-ssize));
                XmScrollBarSetValues
                        (pane->vsb,value,ssize,inc,page_inc,True);
        }
#if	DEBUG_DATABROWSER
        fprintf(stderr,"new value %d\n",value);
#endif
        

        return NhlNOERROR;

}

extern NhlErrorTypes NgDeletePage(
        int		goid,
        NgPageId	page_id
        )
{
        NgGO		go = (NgGO)_NhlGetLayer(goid);
        brPage		*page = GetPageReference(go,page_id);

        if (! page) {
                NHLPERROR((NhlWARNING,NhlEUNKNOWN,"invalid page specified"));
                return NhlWARNING;
        }

	DeleteVarPage(page);

	return NhlNOERROR;
}

/*
 * this function is used to save the state of a page that is "hidden",
 * (removed from view in the browser, generally by a user action) but 
 * still has a valid reference. If the page is restored this data allows
 * the same page id to be used, and any page-type specific elements to
 * be restored (as implemented in each page type). 
 */

extern NhlErrorTypes NgSavePageState
(
	int			goid,
	int			page_id,
        NrmQuark		qfile,
        NrmQuark		qvar,
	NhlPointer		page_state,
	NhlFreeFunc		page_state_free
	)

{
	NgGO go = (NgGO) _NhlGetLayer(goid);
	NgPageSaveState ps_state;
	NgBrowseClassRec *bclass;
	NgBrowseClassPart *bcp;

	bclass = (NgBrowseClassRec *) go->base.layer_class;
	bcp = &bclass->browse_class;
	if (! bcp->hidden_page_state) {
		bcp->hidden_page_state = XmLArrayNew(0,0);
		if (! bcp->hidden_page_state) {
			NHLPERROR((NhlFATAL,ENOMEM,NULL));
			return NhlFATAL;
		}
	}

	ps_state = NhlMalloc(sizeof(NgPageSaveStateRec));
	if (! ps_state) {
		NHLPERROR((NhlFATAL,ENOMEM,NULL));
		return NhlFATAL;
	}
	ps_state->page_id = page_id;
	ps_state->qvar = qvar;
	ps_state->qfile = qfile;
	ps_state->page_state = page_state;
	ps_state->page_state_free = page_state_free;
	
	XmLArrayAdd(bcp->hidden_page_state,0,1);
	XmLArraySet(bcp->hidden_page_state,0,ps_state);

	return NhlNOERROR;
}

extern void NgPageMessageNotify
(
	int	goid,
	int	page_id
)
{
	brPage *page = _NgGetPageRef(goid,page_id);

	if (! page)
		return;

	if (page->pdata->page_message_notify)
		(*page->pdata->page_message_notify)(page);

	return;
}

extern NhlErrorTypes NgPostPageMessage
(
	int			goid,
	int			from_id,
	NgPageMessageType	reply_req,
	brPageType		to_type,
        NrmQuark		to_qfile,
        NrmQuark		to_qvar,
	NgPageMessageType	mtype,
	NhlPointer		message,
	NhlBoolean		overwrite,
	NhlFreeFunc		message_free,
	NhlBoolean		notify
	)

{
	NgGO go = (NgGO) _NhlGetLayer(goid);
	NgPageMessage pmesg;
	NgBrowseClassRec *bclass;
	NgBrowseClassPart *bcp;
	int i,count,pos = -1,page_id;
	NhlBoolean message_replaced = False;

	/* 
	 * messages belonging to each page type are stored contiguously
	 * to make them easy to retrieve as a single entity. Note the
	 * order is most recent message first. So a message reader that
	 * wants to read messages in the same order as the original messages
	 * should read them backwards.
	 */

	bclass = (NgBrowseClassRec *) go->base.layer_class;
	bcp = &bclass->browse_class;

	if (! bcp->page_messages) {
		bcp->page_messages = XmLArrayNew(0,0);
		if (! bcp->page_messages) {
			NHLPERROR((NhlFATAL,ENOMEM,NULL));
			return NhlFATAL;
		}
	}

	count = XmLArrayGetCount(bcp->page_messages);

	if (overwrite) {
		for (i = 0; i < count; i++) {
			pmesg = XmLArrayGet(bcp->page_messages,i);
			if (pmesg->to_qvar == to_qvar && 
			    pmesg->to_qfile == to_qfile &&
			    pmesg->from_id == from_id &&
			    pmesg->to_type == to_type &&
			    pmesg->mtype == mtype) {
				if (pmesg->message_free)
					(pmesg->message_free)(pmesg->message);
				pmesg->message_free = message_free;
				pmesg->message = message;
				pmesg->reply_req = reply_req;
				message_replaced = True;
				break;
			}
		}
		/* no message to overwrite, so just insert the message */
	}

	if (! message_replaced) {
		for (i = 0; i < count; i++) {
			pmesg = XmLArrayGet(bcp->page_messages,i);
			if (pmesg->to_qvar == to_qvar && 
			    pmesg->to_qfile == to_qfile) {
				pos = i;
				break;
			}
		}
		if (pos < 0)
			pos = 0;

		pmesg = NhlMalloc(sizeof(NgPageMessageRec));
		if (! pmesg) {
			NHLPERROR((NhlFATAL,ENOMEM,NULL));
			return NhlFATAL;
		}
		pmesg->from_id = from_id;
		pmesg->to_type = to_type;
		pmesg->to_qvar = to_qvar;
		pmesg->to_qfile = to_qfile;
		pmesg->mtype = mtype;
		pmesg->message = message;
		pmesg->message_free = message_free;
		pmesg->reply_req = reply_req;
	
		XmLArrayAdd(bcp->page_messages,pos,1);
		XmLArraySet(bcp->page_messages,pos,pmesg);
	}

	if (notify) {
		page_id = NgGetPageId(go->base.id,to_qvar,to_qfile);
		if (page_id > NgNoPage) {
#if 0
			NgResetPage(go->base.id,page_id);
#endif
			NgPageMessageNotify(goid,page_id);
		}
	}

	return NhlNOERROR;
}

            /* returns message count */
extern int NgRetrievePageMessages
(
	int			goid,
	brPageType		to_type,
        NrmQuark		to_qfile,
        NrmQuark		to_qvar,
	NgPageMessage		**messages
	)
{
	NgGO go = (NgGO) _NhlGetLayer(goid);
	NgPageMessage pmesg;
	NgBrowseClassRec *bclass;
	NgBrowseClassPart *bcp;
	int i,start_pos = -1,end_pos,count;

	bclass = (NgBrowseClassRec *) go->base.layer_class;
	bcp = &bclass->browse_class;
	*messages = NULL;
	if (! bcp->page_messages) {
		bcp->page_messages = XmLArrayNew(0,0);
		if (! bcp->page_messages) {
			NHLPERROR((NhlFATAL,ENOMEM,NULL));
			return NhlFATAL;
		}
	}
	count = XmLArrayGetCount(bcp->page_messages);

	for (i = 0; i < count; i++) {
		pmesg = XmLArrayGet(bcp->page_messages,i);
		if (pmesg->to_qvar == to_qvar && pmesg->to_qfile == to_qfile) {
			if (start_pos < 0) {
				end_pos = start_pos = i;
			}
			else {
				end_pos = i;
			}
		}
		if (start_pos > 0)
			break;
	}
	if (start_pos < 0)
		return 0;
	count = end_pos - start_pos + 1;
	*messages = (NgPageMessage*)NhlMalloc(sizeof(NgPageMessage *) * count);

	for (i = 0; i < count; i++) {
		(*messages)[i] = (NgPageMessage)
			XmLArrayGet(bcp->page_messages,start_pos + i);
	}
	XmLArrayDel(bcp->page_messages,start_pos,count);


	return count;
}

extern void NgDeletePageMessages
(
	int		goid,
	int		count,
	NgPageMessage	*messages,
	NhlBoolean	delete_message_data
	)
{
	int i;

	for (i = 0; i < count; i++) {
		if (delete_message_data && messages[i]->message_free)
			(messages[i]->message_free)(messages[i]->message);
		NhlFree(messages[i]);
	}
	NhlFree(messages);
	return;
}


static void TabFocusAction
(
	Widget		w,
	XEvent		*xev,
	String		*params,
	Cardinal	*num_params
)
{
	NgGO	go;
        
#if	DEBUG_DATABROWSER & DEBUG_FOLDER
	fprintf(stderr,"TabFocusAction(IN)\n");
#endif
        
	return;
}
