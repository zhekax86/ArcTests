#include "Stdafx.h"
#include "LZSS.h"

using namespace CLib::Base;
using namespace std;

void LZSS::InitEncoder()
{
	for(size_t i=0;i<65536;i++)
		FirstEntry[i] = -1;
	for(size_t i=0;i<DICTLEN;i++)
		Chain[i] = ForwardChain[i] = 0;
	SingleElemsCount = 0;
	CurrentBlockType = -1;
}

void LZSS::AddInChain(unsigned int c, size_t pos)
{
	int newpos = pos % DICTLEN;
	if( FirstEntry[c] == -1)
		FirstEntry[c] = pos;
	else
	{
		int old_pos = FirstEntry[c], shift = pos-old_pos;
		FirstEntry[c] = pos;
		Chain[newpos] = shift;
		ForwardChain[newpos-shift >= 0? newpos-shift: newpos - shift + DICTLEN] = shift;
	}
}

void LZSS::DeleteLastFromChain(unsigned int c, size_t pos)
{
	int newpos = pos % DICTLEN;
	int shift = ForwardChain[newpos];
	ForwardChain[newpos] = 0;
	if (shift == 0)
		FirstEntry[c] = -1;
	else
	{
		Chain[newpos + shift < DICTLEN ?newpos+shift:newpos+shift-DICTLEN] = 0;
	}
}

void LZSS::WriteChain(unsigned int offset, unsigned int bits)
{
	LZSSElem e = {offset, bits, false};
	if(SingleElemsCount >0)	//Проверим, вдруг у нас много одниночных символов и их надо записать в RAW блок?
	{
		//длина кода EOF (OFFBITS+LENBITS) + 2 бита для нового блока + 8 бит для блока одиночных символов
		unsigned int NewBlockOverHead = OFFBITS+/*LENBITS+*/1 + 2 + 8 + (SingleElemsCount < elems.size() ? OFFBITS /*+ LENBITS*/ + 1 : 0); 
		unsigned int CurrentBlockOverhead = SingleElemsCount;
		if(CurrentBlockOverhead > NewBlockOverHead)	//если расходов на смену блока меньше чем от записи одиночных символов с лишним битом, то делаем новый блок
		{
			size_t position = 0, esize = elems.size();
			if(SingleElemsCount < elems.size())
			{
				out.WriteBits(1,1);	//не последний блок
				out.WriteBits(0,1); //тип блока LZSS
				while(position < (esize - SingleElemsCount))
				{
					if(elems[position].singlechar)
					{
						out.WriteBits(0,1);
						out.WriteBits(elems[position].offset,8);
					}
					else
					{
						out.WriteBits(1,1);
						out.WriteBits(elems[position].offset,OFFBITS);
						out.WriteBits(elems[position].length,LENBITS);
					}
					position++;
				}
				out.WriteBits(1,1);
				out.WriteBits(0,OFFBITS);
				//out.WriteBits(0,LENBITS);
			}

			out.WriteBits(1,1);	//1 - ноый блок не последний
			out.WriteBits(1,1);	//1 - тип блока RAW
			//size_t count = SingleElemsCount > 63? 63 : SingleElemsCount;
			out.WriteBits(SingleElemsCount,6);	//Кол-во символов
			for(size_t i=0;i<SingleElemsCount; i++)
				out.WriteBits(elems[position++].offset,8);
			//SingleElemsCount -= count;

			elems.erase( elems.begin(), elems.begin()+position);
		}
	}
	elems.push_back(e);
	SingleElemsCount = 0;
}

void LZSS::WriteChar(unsigned char c)
{
	LZSSElem e = {c, 0, true};
	elems.push_back(e);
	SingleElemsCount++;
	if(SingleElemsCount > 63)
	{
		auto position = elems.begin();
		if(elems.size() > SingleElemsCount)
		{
			out.WriteBits(1,1); //ноывй блок не последний
			out.WriteBits(0,1); //LZ блок

			auto ep = elems.end() - SingleElemsCount;

			while(position != ep)
			{
				if(position->singlechar)
				{
					out.WriteBits(0,1);
					out.WriteBits(position->offset,8);
				}
				else
				{
					out.WriteBits(1,1);
					out.WriteBits(position->offset,OFFBITS);
					out.WriteBits(position->length,LENBITS);
				}


				position++;
			}
			out.WriteBits(1,1);
			out.WriteBits(0,OFFBITS);
			//out.WriteBits(0,LENBITS);
		}
		
		out.WriteBits(1,1);	//ноывй блок не последний
		out.WriteBits(1,1); //RAW блок
		out.WriteBits(63,6);	//63 символа
		for(int i=0;i<63;i++)
			out.WriteBits( (position++)->offset,8);
		elems.erase(elems.begin(),position);
		SingleElemsCount -= 63;
	}
}

void LZSS::Flush()
{
	if(SingleElemsCount == elems.size())
	{
		out.WriteBits(0,1);	//новый блок последний
		out.WriteBits(1,1); //RAW блок
		out.WriteBits(SingleElemsCount,6);
		for( auto c: elems)
			out.WriteBits(c.offset,8);
	}
	else
	{
		out.WriteBits(0,1);	//новый блок последний
		out.WriteBits(0,1); //LZSS блок
		for(auto c : elems)
		{
			if(c.singlechar)
			{
				out.WriteBits(0,1);
				out.WriteBits(c.offset,8);
			}
			else
			{
				out.WriteBits(1,1);
				out.WriteBits(c.offset,OFFBITS);
				out.WriteBits(c.length,LENBITS);
			}
		}
		out.WriteBits(1,1);
		out.WriteBits(0,OFFBITS);
		//out.WriteBits(0,LENBITS);
	}
	out.Flush();
}

charbuf LZSS::Compress(charbuf &source,  int optimization)
{
	size_t len = source.Length();
	const unsigned char *buf = source.GetBuf();
	//OutBitStream _out(len/2);
	//out = OutBitStream(len/2);
	ofstream logfile("LZSS.log");
	InitEncoder();

	size_t current = 0,dict_size = 2;
	while(current < len)
	{
		int start = current+1 < len? FirstEntry[buf[current] + (buf[current+1]<<8)] : -1;
		int moffset, mlen=1;
		while( start >=0 )
		{
			int i;
			for(i=0; start+i< current && current+i < len && i<BUFLEN && buf[start+i] == buf[current+i]; i++) ;
			if(i>mlen)
			{
				moffset = current-start;
				mlen = i;
			}
			if(Chain[start%DICTLEN] != 0)
				start -= Chain[ start%DICTLEN ];
			else
				start = -1;
		}

		if(mlen >= THRESHOLD)
		{
			switch (optimization)
			{
			case 1:
				WriteChain(moffset,mlen);
				break;
			default:
				out.WriteBits(1,1);
				out.WriteBits(moffset,OFFBITS);
				out.WriteBits(mlen,LENBITS);
				break;
			}
			logfile<<'('<<moffset<<','<<mlen<<')'<<endl;
		}
		else
		{
			mlen = 1;
			switch (optimization)
			{
			case 1:
				WriteChar(buf[current]);
				break;
			case 2:
				out.WriteBits(0,1);
				Huff.EncodeChar(buf[current],out);
				break;
			default:
				out.WriteBits(0,1);
				out.WriteBits(buf[current],8);
				break;
			}
			logfile<<buf[current]<<endl;
		}
		
		while(mlen--)
		{
			if(current >= DICTLEN)
				DeleteLastFromChain(buf[current-DICTLEN]+(buf[current+1-DICTLEN]<<8),current-DICTLEN);
			if(current+1 < len)
				AddInChain(buf[current]+(buf[current+1]<<8),current);
			current++;
		}
		
	}

	switch(optimization)
	{
	case 1:
		Flush();
		break;
	default:
		out.WriteBits(1,1);
		out.WriteBits(0,OFFBITS);
		//out.WriteBits(0,LENBITS);
		out.Flush();
		break;
	}

	return charbuf( move(out.GetCharbuf()) );
}

charbuf LZSS::DeCompress(charbuf &source, int optimization)
{
	BitReader input(source.GetBuf(), source.Length() );
	charbuf newbuf(source.Length() * 2);
	//size_t newpos=0;
	bool MoreBlocks = false;
	int BlockType =0;
	if(optimization == 1)
	{
		MoreBlocks = input.GetBit() == 1;
		BlockType = input.GetBit() == 1;
	}

	while(true)
	{
		if(BlockType == 0)
		{
			while(true)
			{
				if( input.GetBit() )
				{
					int shift = input.GetBits(OFFBITS);
					if( shift==0)
						break;
					int rlen = input.GetBits(LENBITS);
					//if(shift ==0 && rlen ==0)
						//break;
					newbuf.AppendCopyedChars(/*newpos-*/shift,rlen);
					//newpos += rlen;
				}
				else
				{
					if(optimization == 2)
						newbuf.AppendChar( Huff.DecodeChar(input));
					else
						newbuf.AppendChar( input.GetBits(8));
					//newpos++;
				}
			}
		}
		else
		{
			unsigned int count = input.GetBits(6);
			for(unsigned int i=0;i<count;i++)
			{
				newbuf.AppendChar( input.GetBits(8) );
				//newpos++;
			}
		}

		if(!MoreBlocks)
			break;

		if(optimization == 1)
		{
			MoreBlocks = input.GetBit() == 1;
			BlockType = input.GetBit() == 1;
		}
	}

	return move(newbuf);
}

charbuf LZSS::Do(charbuf &source)
{
	return move(Compress(source,0));
}

charbuf LZSS::UnDo(charbuf &source)
{
	return move(DeCompress(source,0));
}