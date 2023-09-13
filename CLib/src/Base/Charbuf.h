#pragma once

#include <string>

namespace CLib::Base
{
	//Символный буфер, передается от одного алгоритма к другому
	//Умеет загружаться из файла, сохраняться в файл
	//Есть доп. функции по добавлению символов или для дополнения цепочкой символов из этого же буфера
	//из самого буфера (для LZ алгоритмов)
	class charbuf
	{
	private:
		unsigned char* buf;
		size_t _length, _capacity;
		void ExpandCapacity();	//Увеличивает размер внутреннего буфера если надо добавить новый символ, а место уже кончилось

	public:
		charbuf(int capacity);
		charbuf(const std::wstring& file);
		charbuf(const charbuf& src);
		charbuf(charbuf&& src) noexcept;
		charbuf(unsigned char* b, int len);
		~charbuf();
		charbuf& operator=(charbuf&& src) noexcept;

		void SaveTo(const std::wstring& file);
		size_t Length();
		//void Length(int l);
		unsigned char* GetBuf();	//Выдает указатель на внутренний буфер, для быстрого доступа к нему

		void AppendChar(unsigned char c);	//Добавить символ
		//Добавить к буферу цепочку символов, скопированную по смещению shift от конца буфера (для LZ)
		void AppendCopyedChars(size_t shift, size_t count);
	};
}