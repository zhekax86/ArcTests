#pragma once

#include "Stdafx.h"

using namespace std;

//��������� �����, ���������� �� ������ ��������� � �������
//����� ����������� �� ����� ����������� � ����
//���� ���. ������� �� ����������� �������� ��� ��� ���������� �������� ��������
//�� ������ ������ (��� LZ ����������)
class charbuf
{
private:
	unsigned char *buf;
	size_t _length,_capacity;
	void ExpandCapacity();

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
	unsigned char * GetBuf();

	void AppendChar(unsigned char c);
	void AppendCopyedChars(size_t shift, size_t count);
};

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
	void WriteEliaseCode(uint num);
};

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

class Act
{
public:
	virtual charbuf Do(charbuf &source);
	virtual charbuf UnDo(charbuf &source);
};

//������� ������� ��� ����� ��� ����������� ������ �����
int NeedBits(unsigned int num);

CLIB_API void _Compress(const wstring &infile, const wstring &outname,const vector<int> &actions, bool Dump);
CLIB_API void _Decompress(const wstring &infile, const wstring &outname,const vector<int> &actions, bool Dump);

unique_ptr<Act> GetAction(int actnum);
CLIB_API void GetActions(vector<wstring> &list);