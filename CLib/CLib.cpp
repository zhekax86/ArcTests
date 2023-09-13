// CLib.cpp : Defines the exported functions for the DLL application.

#include "stdafx.h"
#include "CLib.h"
#include "base.h"

#include "BWT.h"
#include "ParallelBWT.h"
#include "Delta.h"
#include "RLE12.h"
#include "RLE.h"
#include "MTF.h"
#include "AdHuffman.h"
#include "Huffman.h"
#include "LZSS.h"
#include "LZSSRAW.h"
#include "LZSSAdHuf.h"
#include "Probability.h"
#include "LZB.h"
#include "LZ78.h"
#include "Arythmetic.h"


void _Compress(const std::wstring& infile, const std::wstring& outname, const std::vector<int>& actions, bool Dump)
{
	charbuf buf(infile);
	unique_ptr<Act> action;

	for (size_t i = 0; i < actions.size();++i)
		if (actions[i] != 0)
		{
			action = GetAction(actions[i]);
			buf = action->Do(buf);
			if (Dump)
			{
				wostringstream dumpname;
				dumpname << infile << L'.' << i << L"_dump";
				buf.SaveTo(dumpname.str());
			}
		}

	buf.SaveTo(outname);
}

void _Decompress(const std::wstring& infile, const std::wstring& outname, const std::vector<int>& actions, bool Dump)
{
	charbuf buf(infile);
	unique_ptr<Act> action;

	for (int i = actions.size() - 1; i >= 0;--i)
		if (actions[i] != 0)
		{
			action = GetAction(actions[i]);
			buf = action->UnDo(buf);
			if (Dump)
			{
				wostringstream dumpname;
				dumpname << infile << L'.' << i << L"_dump";
				buf.SaveTo(dumpname.str());
			}
		}

	buf.SaveTo(outname);
}

unique_ptr<Act> GetAction(int actnum)
{
	switch (actnum)
	{
	case 1:	return unique_ptr<Act>(new BWT());
	case 2:	return unique_ptr<Act>(new ParallelBWT());
	case 3: return unique_ptr<Act>(new Delta());
	case 4: return unique_ptr<Act>(new RLE12());
	case 5: return unique_ptr<Act>(new RLE());
	case 6: return unique_ptr<Act>(new MTF());
	case 7: return unique_ptr<Act>(new Huffman());
	case 8: return unique_ptr<Act>(new AdHuffman());
	case 9: return unique_ptr<Act>(new LZSS());
	case 10: return unique_ptr<Act>(new LZSSRAW());
	case 11: return unique_ptr<Act>(new LZSSAdHuf());
	case 12: return unique_ptr<Act>(new Probability());
	case 13: return unique_ptr<Act>(new LZB());
	case 14: return unique_ptr<Act>(new LZ78());
	case 15: return unique_ptr<Act>(new Arythmetic());
	};
	return unique_ptr<Act>(new Act());
}

void GetActions(std::vector<std::wstring>& list)
{
	list.push_back(L"Ничего");
	list.push_back(L"Barrows Wheeler Transform");
	list.push_back(L"Barrows Wheeler Transform (parallel)");
	list.push_back(L"Delta");
	list.push_back(L"RLE 1+2 bytes");
	list.push_back(L"RLE");
	list.push_back(L"Move To Front");
	list.push_back(L"Huffman");
	list.push_back(L"Adaptive Huffman");
	list.push_back(L"LZSS");
	list.push_back(L"LZSS + RAW");
	list.push_back(L"LZSS + Adaptive Huffman for single chars");
	list.push_back(L"Вероятностное сжатие");
	list.push_back(L"LZB");
	list.push_back(L"LZ78");
	list.push_back(L"Адаптивное арифметическое кодирование");
}

std::vector<std::wstring> GetActions()
{
	std::vector<std::wstring> vec;
	GetActions(vec);
	return vec;
 }