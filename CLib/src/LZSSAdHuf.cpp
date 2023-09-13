#include "Stdafx.h"
#include "LZSSAdHuf.h"


using namespace CLib::Base;

charbuf LZSSAdHuf::Do(charbuf &source)
{
	return std::move(coder.Compress(source,2));
}

charbuf LZSSAdHuf::UnDo(charbuf &source)
{
	return std::move(coder.DeCompress(source,2));
}