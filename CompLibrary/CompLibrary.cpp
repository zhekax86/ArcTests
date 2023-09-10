// This is the main DLL file.

#include "stdafx.h"

#include "CompLibrary.h"
#include <vcclr.h>
#include "..\CLib\Base.h"
#include <vector>
using namespace std;

void CompLibrary::Compressor::Compress(String ^inputname, String ^outputname, array<int>^ Actions, bool Dump)
{
	pin_ptr<const wchar_t> str = PtrToStringChars(inputname);
	wstring infile(str);
	str = PtrToStringChars(outputname);
	wstring outfile(str);
	str = nullptr;

	vector<int> act;
	int len = Actions->Length;
	act.reserve(len);
	for(int i=0;i<len;i++)
		act.push_back(Actions[i]);

	_Compress(infile,outfile,act,Dump);
}

void CompLibrary::Compressor::Decompress(String ^inputname, String ^outputname, array<int> ^Actions, bool Dump)
{
	pin_ptr<const wchar_t> str = PtrToStringChars(inputname);
	wstring infile(str);
	str = PtrToStringChars(outputname);
	wstring outfile(str);
	str = nullptr;

	vector<int> act;
	int len = Actions->Length;
	act.reserve(len);
	for(int i=0;i<len;i++)
		act.push_back(Actions[i]);

	_Decompress(infile,outfile,act,Dump);
}

array<String^>^ CompLibrary::Compressor::GetActionList()
{
	vector<wstring> nlist;
	GetActions(nlist);

	array<String^>^ list = gcnew array<String^>(nlist.size());
	for(size_t i=0;i<nlist.size();i++)
		list[i] = gcnew String(nlist[i].c_str());
	return list;
}