#include "Stdafx.h"
#include "LZSSRAW.h"


using namespace CLib::Base;
using namespace std;

charbuf LZSSRAW::Do(charbuf &source)
{
	return move(coder.Compress(source,1));
}

charbuf LZSSRAW::UnDo(charbuf &source)
{
	return move(coder.DeCompress(source,1));
}