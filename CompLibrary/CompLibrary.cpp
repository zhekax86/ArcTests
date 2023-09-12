// This is the main DLL file.

#include "CompLibrary.h"
#include "..\CLib\CLib.h"
#include <vcclr.h>
#include <vector>

void CompLibrary::Compressor::Compress(String^ inputname, String^ outputname, array<int>^ Actions, bool Dump)
{
	pin_ptr<const wchar_t> str = PtrToStringChars(inputname);
	std::wstring infile(str);
	str = PtrToStringChars(outputname);
	std::wstring outfile(str);
	str = nullptr;

	std::vector<int> act;
	int len = Actions->Length;
	act.reserve(len);
	for (int i = 0;i < len;i++)
		act.push_back(Actions[i]);

	_Compress(infile, outfile, act, Dump);
}

void CompLibrary::Compressor::Decompress(String^ inputname, String^ outputname, array<int>^ Actions, bool Dump)
{
	pin_ptr<const wchar_t> str = PtrToStringChars(inputname);
	std::wstring infile(str);
	str = PtrToStringChars(outputname);
	std::wstring outfile(str);
	str = nullptr;

	std::vector<int> act;
	int len = Actions->Length;
	act.reserve(len);
	for (int i = 0;i < len;i++)
		act.push_back(Actions[i]);

	_Decompress(infile, outfile, act, Dump);
}

array<String^>^ CompLibrary::Compressor::GetActionList()
{
	auto nlist = GetActions();

	array<String^>^ list = gcnew array<String^>(nlist.size());
	for (size_t i = 0;i < nlist.size();i++)
		list[i] = gcnew String(nlist[i].c_str());

	return list;
}