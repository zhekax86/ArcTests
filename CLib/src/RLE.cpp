#include "Stdafx.h"
#include "RLE.h"

using namespace CLib::Base;
using namespace std;

charbuf RLE::Do(charbuf &source)
{
	size_t len = source.Length();
	const unsigned char *buf = source.GetBuf();
	charbuf newbuf(len);

	int freq[256];
	for(size_t i=0;i<256;i++)
		freq[i] = 0;

	for(size_t i=0;i<len;i++)
	{
		int ind = (buf[i]);
		freq[ind]++;
	}

	int minind = 0;
	for(int i=1;i<256;i++)
	{
		if( freq[i] < freq[minind])
			minind = i;
	}

	unsigned char repeatchar = minind;
	newbuf.AppendChar(repeatchar);
	size_t position = 0;

	while(position < len)
	{
		if(position +3 < len)
		{
			unsigned char cc = buf[position];
			if(cc == buf[position+1] && cc == buf[position+2] && cc == buf[position+3])
			{
				int SampleLen = 4;
				while(position+SampleLen < len && SampleLen <256 && buf[position+SampleLen]==cc)
				{
					SampleLen++;
				}
				//SampleLen--;
				newbuf.AppendChar(repeatchar);
				newbuf.AppendChar((unsigned char)(SampleLen-1));
				newbuf.AppendChar(cc);
				position += SampleLen -1;
			}
			else
			{
				if(cc != repeatchar)
					newbuf.AppendChar(cc);
				else
				{
					newbuf.AppendChar(repeatchar);
					newbuf.AppendChar(0);
					newbuf.AppendChar(repeatchar);
				}

			}
		}
		else
		{
			if(buf[position] != repeatchar)
				newbuf.AppendChar(buf[position]);
			else
			{
				newbuf.AppendChar(repeatchar);
				newbuf.AppendChar(0);
				newbuf.AppendChar(repeatchar);
			}
		}
		position++;
	}


	return move(newbuf);
}

charbuf RLE::UnDo(charbuf &source)
{
	size_t len = source.Length();
	const unsigned char *buf = source.GetBuf();
	unsigned char repeatchar = buf[0];
	charbuf newbuf(len);
	size_t position = 1;

	while(position < len)
	{
		unsigned char cc = buf[position++];
		if(cc != repeatchar)
			newbuf.AppendChar(cc);
		else
		{
			int repeats = (int)(buf[position++])+1;
			unsigned char rc = buf[position++];
			for(int i=0;i<repeats;i++)
				newbuf.AppendChar(rc);
		}
	}

	return move(newbuf);
}

