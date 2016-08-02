#include "Stdafx.h"
#include "LZSSRAW.h"


//LZSSRAW::LZSSRAW

charbuf LZSSRAW::Do(charbuf &source)
{
	return move(coder.Compress(source,1));
}

charbuf LZSSRAW::UnDo(charbuf &source)
{
	return move(coder.DeCompress(source,1));
}