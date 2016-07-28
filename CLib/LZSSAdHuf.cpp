#include "Stdafx.h"
#include "LZSSAdHuf.h"


//LZSSRAW::LZSSRAW

charbuf LZSSAdHuf::Do(charbuf &source)
{
	return move(coder.Compress(source,2));
}

charbuf LZSSAdHuf::UnDo(charbuf &source)
{
	return move(coder.DeCompress(source,2));
}