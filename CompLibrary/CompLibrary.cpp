// This is the main DLL file.

#include "CompLibrary.h"
#include "..\CLib\CLib.h"
#include <vcclr.h>
#include <vector>

void CompLibrary::Compressor::Compress(String^ inputname, String^ outputname, array<int>^ actions, bool dump)
{
	pin_ptr<const wchar_t> input_str = PtrToStringChars(inputname);
	pin_ptr<const wchar_t> output_str = PtrToStringChars(outputname);
	pin_ptr<const int> actions_ptr = &actions[0];

	_Compress(input_str, output_str, actions_ptr, actions->Length, dump);
}

void CompLibrary::Compressor::Decompress(String^ inputname, String^ outputname, array<int>^ actions, bool dump)
{
	pin_ptr<const wchar_t> input_str = PtrToStringChars(inputname);
	pin_ptr<const wchar_t> output_str = PtrToStringChars(outputname);
	pin_ptr<const int> actions_ptr = &actions[0];

	_Decompress(input_str, output_str, actions_ptr, actions->Length, dump);
}

array<String^>^ CompLibrary::Compressor::GetActionList()
{
	auto nlist = GetActions();

	array<String^>^ list = gcnew array<String^>(nlist.size());
	for (size_t i = 0;i < nlist.size();i++)
		list[i] = gcnew String(nlist[i].c_str());

	return list;
}