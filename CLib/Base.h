#pragma once

#include "Stdafx.h"

using namespace std;

//Символный буфер, передается от одного алгоритма к другому
//Умеет загружаться из файла, сохраняться в файл
//Есть доп. функции по добавлению символов или для дополнения цепочкой символов из этого же буфера
//из самого буфера (для LZ алгоритмов)
class charbuf
{
private:
	unsigned char *buf;
	size_t _length,_capacity;
	void ExpandCapacity();	//Увеличивает размер внутреннего буфера если надо добавить новый символ, а место уже кончилось

public:
	charbuf(int capacity);
	charbuf(const wstring &file);
	charbuf(const charbuf &src);
	charbuf(charbuf &&src);
	charbuf(unsigned char *b, int len);
	~charbuf();
	charbuf& operator=(charbuf &&src);

	void SaveTo(const wstring &file);
	size_t Length();
	//void Length(int l);
	unsigned char * GetBuf();	//Выдает указатель на внутренний буфер, для быстрого доступа к нему

	void AppendChar(unsigned char c);	//Добавить символ
	//Добавить к буферу цепочку символов, скопированную по смещению shift от конца буфера (для LZ)
	void AppendCopyedChars(size_t shift, size_t count);	
};

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
	void WriteEliaseCode(uint num);	//кодирует положительное целое число специальным кодом переменной длинны и сразу пишет
};

//Побитовый ввод.
class BitReader
{
private:
	unsigned char *buf;
	size_t cur_char,buf_len;
	unsigned int cur_bit;
	unsigned int bytes_after_end;
public:
	BitReader(unsigned char *c, size_t len);
	BitReader(unsigned char *c, size_t len, unsigned int after_end);
	unsigned int GetBits(unsigned int count);
	unsigned char GetBit();
	uint ReadEliaseCode();
};

//Базовый класс для какой-либо обработки charbuf
//Все алгоритмы наследуются от него
class Act
{
public:
	virtual charbuf Do(charbuf &source);
	virtual charbuf UnDo(charbuf &source);
};

//Говорит сколько бит нужно для кодирования такого числа (для кодов Элиаса)
int NeedBits(unsigned int num);

CLIB_API void _Compress(const wstring &infile, const wstring &outname,const vector<int> &actions, bool Dump);
CLIB_API void _Decompress(const wstring &infile, const wstring &outname,const vector<int> &actions, bool Dump);

unique_ptr<Act> GetAction(int actnum);
CLIB_API void GetActions(vector<wstring> &list);