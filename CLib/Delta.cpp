#pragma unmanaged

#include "Stdafx.h"
#include "Delta.h"

charbuf Delta::Do(charbuf &source)
{
	int len = source.Length();
	unsigned char *buf = source.GetBuf();

	unsigned char lastchar = buf[0];
	for(int i=1;i<len;i++)
	{
		char c = buf[i];
		buf[i]-=lastchar;
		lastchar = c;
	}

	/*for(int i=1;i<len;i++)
	{
		buf[i] -= buf[i-1];
	}*/

	return move(source);
}

charbuf Delta::UnDo(charbuf &source)
{
	int len = source.Length();
	unsigned char *buf = source.GetBuf();

	for(int i=1;i<len;i++)
		buf[i]+=buf[i-1];
	/*char lastchar = buf[0];
	for(int i=1;i<len;i++)
	{
		char c = buf[i];
		buf[i]+=lastchar;
		lastchar = c;
	}*/

	return move(source);
}