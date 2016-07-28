// CompLibrary.h

#pragma once

using namespace System;

namespace CompLibrary {

	public ref class Compressor
	{
		// TODO: Add your methods for this class here.
	public:
		static void Compress(String ^inputname, String ^outputname, array<int> ^Actions, bool Dump);
		static void Decompress(String ^inputname, String ^outputname, array<int> ^Actions, bool Dump);
		static array<String^>^ GetActionList();
	};
}
