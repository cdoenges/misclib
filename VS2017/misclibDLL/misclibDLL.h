// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MISCLIBDLL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MISCLIBDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MISCLIBDLL_EXPORTS
#define MISCLIBDLL_API __declspec(dllexport)
#else
#define MISCLIBDLL_API __declspec(dllimport)
#endif

// This class is exported from the misclibDLL.dll
class MISCLIBDLL_API CmisclibDLL {
public:
	CmisclibDLL(void);
	// TODO: add your methods here.
};

extern MISCLIBDLL_API int nmisclibDLL;

MISCLIBDLL_API int fnmisclibDLL(void);
