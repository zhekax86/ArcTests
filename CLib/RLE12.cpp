#pragma unmanaged

#include "Stdafx.h"
#include "RLE12.h"


charbuf RLE12::Do(charbuf &source)
{
	size_t len = source.Length();
	const unsigned char *buf = source.GetBuf();
	charbuf newbuf(len);

	//символы для повторов (1 байт и два)
	unsigned char rep1, rep2;
	int stat[256];
	for(int i=0;i<256;i++)
		stat[i]=0;

	for(size_t i=0;i<len;i++)
		stat[buf[i]]++;

	if(stat[0]<stat[1])
	{
		rep1 = 0;
		rep2 = 1;
	}
	else
	{
		rep1=1;
		rep2=0;
	}

	for(int i=2;i<256;i++)
		if(stat[i] <= stat[rep1])
		{
			rep2=rep1;
			rep1=i;
		}
		
	size_t position=0;
	newbuf.AppendChar(rep1);
	newbuf.AppendChar(rep2);

	while(position < len)
	{
		size_t /*reps = 0,*/ p = 1;
		unsigned char rc = buf[position];
		while(position+p < len && buf[position+p] == rc)
			p++;
		//p--;

		if(p>3)
		{
			if(p<=255)
			{
				newbuf.AppendChar(rep1);
				newbuf.AppendChar(p);
				newbuf.AppendChar(rc);
				position += p;
			}
			else
			{
				p = p>0xffff? 0xffff:p;
				newbuf.AppendChar(rep2);
				newbuf.AppendChar(p&0x00ff);
				newbuf.AppendChar(p>>8);
				newbuf.AppendChar(rc);
				position +=p;
			}
		}
		else
		{
			if(rc != rep1 && rc != rep2)
			{
				newbuf.AppendChar(rc);
				position++;
			}
			else
			{
				newbuf.AppendChar(rc);
				newbuf.AppendChar( p==1? 0: p);
				if(rc == rep2)
					newbuf.AppendChar(0);
				position +=p;
			}
		}

	}



	/*size_t position = 0, startposition = 0, blocklength = 0;
	bool repeatingmode;
	if(buf[0] == buf[1])
		repeatingmode = true;
	else
		repeatingmode = false;

	while(position < len)
	{
		if(position + 1 < len)
		{
			startposition = position;
			blocklength = 1;

			if(buf[position] != buf[position+1])
			{
				position++;
				while(position+1 < len && blocklength<=128 && buf[position] != buf[position+1])
				{
					blocklength++;
					position++;
				}
				if(position == len-1 && buf[position] != buf[position-1])
				{
					blocklength++;
					position++;
				}

				//blocklength--;
				unsigned char c = 0x00 | (unsigned char)(blocklength-1);
				newbuf.AppendChar(c);
				while(startposition < position)
					newbuf.AppendChar( buf[startposition++]);

			}
			else
			{
				unsigned char p = buf[position++];
				blocklength++;
				while(position < len && blocklength <= 128 && buf[position] == p)
				{
					blocklength++;
					position++;
				}
				blocklength--;

				unsigned char c = 0x80 | (unsigned char)(blocklength-1);
				newbuf.AppendChar(c);
				newbuf.AppendChar(p);
			}
		}
		else
		{
			newbuf.AppendChar(0x80);
			newbuf.AppendChar(buf[position++]);
		}
	}*/

	return move(newbuf);
}

charbuf RLE12::UnDo(charbuf &source)
{
	size_t len = source.Length();
	const unsigned char *buf = source.GetBuf();
	charbuf newbuf(len*2);

	unsigned char rep1=buf[0],rep2=buf[1];
	size_t position=2;

	while(position < len)
	{
		unsigned char c = buf[position++];
		if(c != rep1 && c!= rep2)
			newbuf.AppendChar(c);
		else
		{
			size_t count = buf[position++];
			if(c == rep2)
				count |= buf[position++] << 8;
			if(count == 0)
				newbuf.AppendChar(c);
			else
			{
				c = buf[position++];
				for(size_t i=0;i<count;i++)
					newbuf.AppendChar(c);
			}
		}
		//postion++;
	}


	/*size_t position = 0;
	while(position < len)
	{
		unsigned char c = buf[position++], flag = c&0x80;
		int repeat = c&0x7f;
		repeat++;
		if( flag != 0)
		{
			unsigned char rc = buf[position++];
			for(int i=0; i<repeat;i++)
				newbuf.AppendChar(rc);
		}
		else
		{
			for(int i=0;i<repeat;i++)
				newbuf.AppendChar(buf[position++]);
		}
	}*/

	return move(newbuf);
}