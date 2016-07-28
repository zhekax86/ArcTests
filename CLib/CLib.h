// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CLIB_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CLIB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef CLIB_EXPORTS
#define CLIB_API __declspec(dllexport)
#else
#define CLIB_API __declspec(dllimport)
#endif

// This class is exported from the CLib.dll
/*class CLIB_API CCLib {
public:
	CCLib(void);
	// TODO: add your methods here.
};

extern CLIB_API int nCLib;

CLIB_API int fnCLib(void);*/
