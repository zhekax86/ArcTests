#include "Stdafx.h"
#include "Base.h"

#pragma once

struct HTreeNode
{
	int count,parent,left,right;	//left ==-1 && right == -1 ->������ ��� ���� ������
	int c; //0-255 - ������, -1 - eof, -2 - escape
	//unsigned char special;	//1 - eof, 2 - escape, 0 - ������� ������
};

class AdHuffman : public Act
{
private:
	vector<HTreeNode> tree;
	int CharPosition[256];	//������ ����� � �������� (-1 - ������ � ������ ������ ������� ���)
	int SpecCharPosition[2]; //������ ����� ��� ������������

	void _DumpTree(size_t i, bool Do);	//��� �������

	int GetStartNode(int ch);
	void AddChar(int nc);
	void UpdateChar(int nc);
	void UpdateCharLite(int nc);
	void GetCode(int ch, unsigned int &code, unsigned int &len);	//������ ��� ��� ����������� �������
	int NextSym(BitReader &input);	//������ ��������� ������ �� ��������������� ������
public:
	AdHuffman();

	virtual charbuf Do(charbuf &source) override;
	virtual charbuf UnDo(charbuf &source) override;

	void EncodeChar(int ch,OutBitStream &stream);
	int DecodeChar(BitReader &stream);
};