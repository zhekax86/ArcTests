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

class LZSS : public Act
{
private:
	int FirstEntry[65536];	//��������� �� ������ ��������� � ������� (-1 - ��� ���������)
	int Chain[DICTLEN];	    //������� �������� ��� ���������
	int ForwardChain[DICTLEN]; //������� �������� ������
	vector<LZSSElem> elems;	//�������� �� �����
	size_t SingleElemsCount;	//���������� ��������� ��������� ��������
	int CurrentBlockType;	//������� ���� -1 - ��� �����, 0 - LZSS, 1 - RAW
	OutBitStream out;	//�������� ����� ��� ������
	AdHuffman Huff;	//������ ������� ��� ����������� ��������� ��������
	
	//ofstream logfile("LZSS.log");	//��� ��������

	void InitEncoder();
	void AddInChain(unsigned int c, size_t pos);
	void DeleteLastFromChain(unsigned int c, size_t pos);
	
	void WriteChain(unsigned int offset, unsigned int bits);
	void WriteChar(unsigned char c);
	void Flush();
public:
	virtual charbuf Do(charbuf &source) override;
	virtual charbuf UnDo(charbuf &source) override;

	charbuf Compress(charbuf &source, int optimization);
	charbuf DeCompress(charbuf &source, int optimization);
};