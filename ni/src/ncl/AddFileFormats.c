#ifdef __cpluplus
extern "C" {
#endif
#include <stdio.h>
#include <ncarg/hlu/hlu.h>
#include <ncarg/hlu/NresDB.h>

#include "defs.h"
#include "NclDataDefs.h"
#include "NclFileInterfaces.h"

extern NclFormatFunctionRecPtr HDFEOSAddFileFormat(
#if	NhlNeedProto
void
#endif
);
extern NclFormatFunctionRecPtr NetCdfAddFileFormat(
#if	NhlNeedProto
void
#endif
);
extern NclFormatFunctionRecPtr HDFAddFileFormat(
#if	NhlNeedProto
void
#endif
);

extern NclFormatFunctionRecPtr GribAddFileFormat(
#if	NhlNeedProto
void
#endif
);
extern NclFormatFunctionRecPtr CcmAddFileFormat(
#if	NhlNeedProto
void
#endif
);

void _NclAddFileFormats
#if	NhlNeedProto
(void)
#else 
()
#endif
{
	_NclRegisterFormat(NetCdfAddFileFormat,"cdf");
	_NclRegisterFormat(NetCdfAddFileFormat,"nc");
	_NclRegisterFormat(HDFAddFileFormat,"hdf");
	_NclRegisterFormat(HDFEOSAddFileFormat,"hdfeos");
	_NclRegisterFormat(HDFAddFileFormat,"hd");
	_NclRegisterFormat(GribAddFileFormat,"grb");
	_NclRegisterFormat(CcmAddFileFormat,"ccm");
	return;
}


#ifdef __cpluplus
}
#endif
