#include "Stdafx.h"
#include "BitReader.h"

using namespace CLib::Base;

BitReader::BitReader(unsigned char *c, size_t len) : buf(c), buf_len(len), cur_char(0), cur_bit(8), bytes_after_end(0)
{
}

BitReader::BitReader(unsigned char *c, size_t len, unsigned int after_end) : buf(c), buf_len(len), cur_char(0), cur_bit(8), bytes_after_end(after_end)
{
}

unsigned int BitReader::GetBits(unsigned int count)
{
	unsigned int result=0, cur_count = count;
	while(cur_count >0)
	{
		unsigned int tempc = cur_count > cur_bit? cur_bit: cur_count;
		unsigned char c = buf[cur_char] << (8-cur_bit);
		unsigned int t = c>>(8-tempc);
		cur_bit -= tempc;
		if(cur_bit == 0)
		{
			cur_char++;
			cur_bit = 8;
		}
		result |= t << (cur_count-tempc);

		cur_count -= tempc;
	}

	return result;
}

unsigned char BitReader::GetBit()
{
	assert(cur_char < buf_len + bytes_after_end);
	unsigned char res;

	if(cur_char < buf_len)
		res = (buf[cur_char] >> (cur_bit-1)) & 0x01;
	else
		res = 0;

	cur_bit--;
	if(cur_bit == 0)
	{
		cur_char ++;
		cur_bit = 8;
	}
	return res;
}

unsigned int BitReader::ReadEliaseCode()
{
	unsigned int count=0,bit = GetBit();
	while(bit == 0)
	{
		bit = GetBit();
		count++;
	}
	unsigned int num=0;
	if(count>0)
		num = GetBits(count);
	num = (bit << count)+num;
	return num;
}


