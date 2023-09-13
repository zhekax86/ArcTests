#pragma once

#include<string>
#include<vector>

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


template<class T>
struct array_with_size
{
public:
	const T* const* const pointer;
	int size;

	array_with_size(const T* const* const pointer, int size)
		: pointer(pointer), size(size) {};
};



CLIB_API void _Compress(const wchar_t* infile, const wchar_t* outname, const int* actions, int actionsCount, bool dump);
CLIB_API void _Decompress(const wchar_t* infile, const wchar_t* outname, const int* actions, int actionsCount, bool dump);

CLIB_API void _Compress(const std::wstring &infile, const std::wstring &outname, const std::vector<int>& actions, bool dump);
CLIB_API void _Decompress(const std::wstring &infile, const std::wstring &outname, const std::vector<int>& actions, bool dump);

CLIB_API void GetActions(std::vector<std::wstring>& list);
CLIB_API std::vector<std::wstring> GetActions();
CLIB_API array_with_size<wchar_t> GetActionsArr();
