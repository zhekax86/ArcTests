#pragma once
using namespace System;

namespace CompLibrary {

	public ref class Compressor
	{
		// TODO: Add your methods for this class here.
	public:
		static void Compress(String^ inputname, String^ outputname, array<int>^ actions, bool dump);
		static void Decompress(String^ inputname, String^ outputname, array<int>^ actions, bool dump);
		static array<String^>^ GetActionList();
	};
}
