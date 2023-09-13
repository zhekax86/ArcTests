#pragma once

namespace CLib::Base
{
	//Побитовый ввод.
	class BitReader
	{
	private:
		unsigned char* buf;
		size_t cur_char, buf_len;
		unsigned int cur_bit;
		unsigned int bytes_after_end;
	public:
		BitReader(unsigned char* c, size_t len);
		BitReader(unsigned char* c, size_t len, unsigned int after_end);
		unsigned int GetBits(unsigned int count);
		unsigned char GetBit();
		unsigned int ReadEliaseCode();
	};
}