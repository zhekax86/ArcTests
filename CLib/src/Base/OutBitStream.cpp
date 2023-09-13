#include "../stdafx.h"
#include "OutBitStream.h"
#include "../Base.h"

namespace CLib::Base
{
	OutBitStream::OutBitStream() : buf(65535), bitsleft(8), curbyte(0)
	{
	}

	OutBitStream::OutBitStream(int bufsize) : buf(bufsize), bitsleft(8), curbyte(0)
	{
	}

	void OutBitStream::WriteBits(unsigned int b, unsigned int count)
	{
		int btw = count;
		unsigned int bl = b;
		while (btw > 0)
		{
			unsigned int tb = bl;
			int bw;
			if (btw > bitsleft)
			{
				tb >>= btw - bitsleft;
				bw = bitsleft;
			}
			else
			{
				bw = btw;
				tb <<= bitsleft - bw;
			}

			curbyte |= tb;
			bitsleft -= bw;
			btw -= bw;
			bl <<= 32 - btw;
			bl >>= 32 - btw;
			if (bitsleft == 0)
			{
				buf.AppendChar(curbyte);
				curbyte = 0;
				bitsleft = 8;
			}
		}
	}

	void OutBitStream::Flush()
	{
		if (bitsleft < 8)
			buf.AppendChar(curbyte);
	}

	charbuf& OutBitStream::GetCharbuf()
	{
		return buf;
	}

	void OutBitStream::WriteEliaseCode(unsigned int num)
	{
		uint nb = NeedBits(num);
		if (nb > 1)
			WriteBits(0, nb - 1);
		WriteBits(num, nb);
	}
}