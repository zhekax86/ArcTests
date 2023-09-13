#pragma once

#include "Charbuf.h"

namespace CLib::Base
{
	//Побитовый вывод. Пишет все во внутренний charbuf
	//Перед тем как забрить из него charbuf для передачи слудующему алгоритму нужно обязательно вызвать flush
	class OutBitStream
	{
	private:
		charbuf buf;
		//size_t current_byte;
		int bitsleft;
		unsigned char curbyte;
	public:
		OutBitStream();
		OutBitStream(int bufsize);
		void WriteBits(unsigned int b, unsigned int count);
		void Flush();
		charbuf& GetCharbuf();
		void WriteEliaseCode(unsigned int num);	//кодирует положительное целое число специальным кодом переменной длинны и сразу пишет
	};
}
