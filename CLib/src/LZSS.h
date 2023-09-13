#include "Stdafx.h"
#include "Base.h"
#include "AdHuffman.h"

#pragma once

#define DICTLEN 4095
#define BUFLEN 127

#define OFFBITS 12
#define LENBITS 7
#define THRESHOLD 3

struct LZSSElem
{
	unsigned int offset,length;
	bool singlechar;
};

class LZSS : public CLib::Base::Act
{
private:
	int FirstEntry[65536];	//указатель на первое вхождение в массиве (-1 - нет вхождений)
	int Chain[DICTLEN];	    //цепочка смещений для вхождения
	int ForwardChain[DICTLEN]; //цепочка смещений вперед
	std::vector<LZSSElem> elems;	//Элементы на выход
	size_t SingleElemsCount;	//Количество последних одиночных символов
	int CurrentBlockType;	//текущий блок -1 - нет блока, 0 - LZSS, 1 - RAW
	CLib::Base::OutBitStream out;	//Выходной поток для кодера
	AdHuffman Huff;	//дерево хафмана для оптимизации одиночных символов
	
	//ofstream logfile("LZSS.log");	//Лог энкодера

	void InitEncoder();
	void AddInChain(unsigned int c, size_t pos);
	void DeleteLastFromChain(unsigned int c, size_t pos);
	
	void WriteChain(unsigned int offset, unsigned int bits);
	void WriteChar(unsigned char c);
	void Flush();
public:
	virtual CLib::Base::charbuf Do(CLib::Base::charbuf &source) override;
	virtual CLib::Base::charbuf UnDo(CLib::Base::charbuf &source) override;

	CLib::Base::charbuf Compress(CLib::Base::charbuf &source, int optimization);
	CLib::Base::charbuf DeCompress(CLib::Base::charbuf &source, int optimization);
};