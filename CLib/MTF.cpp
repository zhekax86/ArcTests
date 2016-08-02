#include "Stdafx.h"
#include "MTF.h"

MTF::MTF()
{
	for(int i=0;i<256;i++)
		symbol[i] = i;
}

inline unsigned char MTF::GetSymbol(unsigned char c)
{
	int i = 0;
	while(symbol[i] != c)
		i++;

	for(int j=i;j>0;j--)
		symbol[j] = symbol[j-1];

	symbol[0] = c;

	return i;
}

inline unsigned char MTF::GetSymbolBackwards(unsigned char c)
{
	unsigned char sym = symbol[c];
	for(int i=c;i>0;i--)
		symbol[i] = symbol[i-1];
	symbol[0] = sym;
	return sym;
}

charbuf MTF::Do(charbuf &source)
{
	size_t len = source.Length();
	const unsigned char *buf = source.GetBuf();
	unsigned char * newbuf = new unsigned char[len];
	//charbuf newbuf(len);

	for(size_t i = 0;i<len;i++)
		newbuf[i] = GetSymbol(buf[i]);

	return charbuf(newbuf,len);
}

charbuf MTF::UnDo(charbuf &source)
{
	size_t len = source.Length();
	const unsigned char *buf = source.GetBuf();
	unsigned char * newbuf = new unsigned char[len];
	//charbuf newbuf(len);

	for(size_t i = 0;i<len;i++)
		newbuf[i] = GetSymbolBackwards(buf[i]);

	return charbuf(newbuf,len);
}