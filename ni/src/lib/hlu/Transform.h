/*
 *      $Id: Transform.h,v 1.15 2000-05-16 01:35:41 dbrown Exp $
 */
/************************************************************************
*									*
*			     Copyright (C)  1992			*
*	     University Corporation for Atmospheric Research		*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		Transform.h
 *
 *	Author:		Ethan Alpert
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Fri Oct 2 16:36:47 MDT 1992
 *
 *	Description:	Public header for Transform class.
 */

#ifndef _NTRANSFORM_h
#define _NTRANSFORM_h

#include <ncarg/hlu/View.h>

typedef enum _NhlOverlayMode {
	NhlDATATRANSFORM = 0,  /* DataTransform */
	NhlNDCVIEWPORT = 1,    /* NDCViewport */
	NhlNDCDATAEXTENT = 2   /* NDCDataExtent */
} NhlOverlayMode;

#define NhlTOverlayMode        "OverlayMode"

/*
 * Public Transform instance resources
 */

#define NhlNtfPlotManagerOn		"tfPlotManagerOn"
#define NhlNtfDoNDCOverlay		"tfDoNDCOverlay"
#define NhlNtfOverlayMode		"tfOverlayMode"
#define NhlNtfLineInterpolationOn 	"tfLineInterpolationOn"

/*
 * Public Transform class resources
 */

#define NhlCtfPlotManagerOn		"TfPlotManagerOn"
#define NhlCtfDoNDCOverlay		"TfDoNDCOverlay"
#define NhlCtfOverlayMode		"TfOverlayMode"
#define NhlCtfLineInterpolationOn 	"TfLineInterpolationOn"

/*
 * Public Functions defined by the Transform Class
 */
extern NhlErrorTypes NhlNDCToData(
#if	NhlNeedProto
	int	/*pid*/,
	float* /*x*/,
	float* /*y*/,
	int	/*n*/,
	float* /*xout*/,
	float* /*yout*/,
	float * /* xmissing */,
	float * /* ymissing */,
	int*	/* status */,
	float*  /* out_of_range */
#endif
);

extern NhlErrorTypes NhlDataToNDC(
#if	NhlNeedProto
	int	/*pid*/,
	float* /*x*/,
	float* /*y*/,
	int	/*n*/,
	float* /*xout*/,
	float* /*yout*/,
	float * /* xmissing */,
	float * /* ymissing */,
	int*	/* status */,
	float*  /* out_of_range */
#endif
);

extern NhlErrorTypes NhlDataPolyline(
#if	NhlNeedProto
	int		/* pid */,
        int		/* gsid */,
	float*		/* x */,
	float*		/* y */,
	int		/* n */
#endif
);

extern NhlErrorTypes NhlNDCPolyline(
#if	NhlNeedProto
	int		/* pid */,
        int		/* gsid */,
	float*		/* x */,
	float*		/* y */,
	int		/* n */
#endif
);

extern NhlErrorTypes NhlDataPolygon(
#if	NhlNeedProto
	int		/* pid */,
        int		/* gsid */,
	float*		/* x */,
	float*		/* y */,
	int		/* n */
#endif
);

extern NhlErrorTypes NhlNDCPolygon(
#if	NhlNeedProto
	int		/* pid */,
        int		/* gsid */,
	float*		/* x */,
	float*		/* y */,
	int		/* n */
#endif
);

extern NhlErrorTypes NhlDataPolymarker(
#if	NhlNeedProto
	int		/* pid */,
        int		/* gsid */,
	float*		/* x */,
	float*		/* y */,
	int		/* n */
#endif
);

extern NhlErrorTypes NhlNDCPolymarker(
#if	NhlNeedProto
	int		/* pid */,
        int		/* gsid */,
	float*		/* x */,
	float*		/* y */,
	int		/* n */
#endif
);

extern NhlBoolean NhlIsTransform(
#if	NhlNeedProto
	int	pid
#endif
);

/* Overlay and Annotation access functions */

extern NhlErrorTypes NhlAddOverlay(
#if	NhlNeedProto
        int		base_id,
	int		transform_id,
	int		after_id
#endif
);

extern NhlErrorTypes NhlRemoveOverlay(
#if	NhlNeedProto
        int		base_id,
	int		overlay_id,
	NhlBoolean	restore
#endif
);

extern int NhlAddAnnotation(
#if	NhlNeedProto
        int	plot_id,
	int	anno_view_id
#endif
);

extern NhlErrorTypes NhlRemoveAnnotation(
#if	NhlNeedProto
        int	plot_id,
	int	anno_manager_id
#endif
);

extern NhlClass NhltransformClass;

#endif /*_NTRANSFORM_h */
