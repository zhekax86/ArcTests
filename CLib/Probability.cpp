#include "Stdafx.h"
#include "Probability.h"

Probability::Probability()
{
	for(int i=0;i<65536;i++)
		PossibleChar[i] = 0;
}

charbuf Probability::Do(charbuf &source)
{
	size_t len = source.Length();
	const unsigned char *buf = source.GetBuf();
	//charbuf newbuf(len/2);
	OutBitStream out(len/2);

	//Пишем длину и первые два символа
	out.WriteBits(len,32);
	out.WriteBits(buf[0],8);
	out.WriteBits(buf[1],8);
	for(size_t i=2;i<len;i++)
	{
		//Если текущий символ совпадает с вероятным, то на выход выдаем 1
		if(PossibleChar[ buf[i-1] + (buf[i-2]<<8) ] == buf[i])
			out.WriteBits(1,1);
		else
		{
			//иначе 0 + текущий символ
			out.WriteBits(0,1);
			out.WriteBits(buf[i],8);
			PossibleChar[ buf[i-1] + (buf[i-2]<<8) ] = buf[i];
		}
	}

	out.Flush();
	return move( out.GetCharbuf() );
}

charbuf Probability::UnDo(charbuf &source)
{
	BitReader input( source.GetBuf(), source.Length());
	charbuf newbuf(source.Length()/2);
	size_t len=input.GetBits(32);
	unsigned char last2=input.GetBits(8),last1=input.GetBits(8),current;

	newbuf.AppendChar(last2);
	newbuf.AppendChar(last1);
	for(size_t i=2;i<len;i++)
	{
		unsigned int p = input.GetBit();
		if(p)
			current = PossibleChar[ last1 + (last2<<8) ] ;
		else
		{
			current = input.GetBits(8);
			PossibleChar[ last1 + (last2<<8) ] = current;
		}
		newbuf.AppendChar( current );
		last2=last1;
		last1=current;
	}

	return move(newbuf);
}