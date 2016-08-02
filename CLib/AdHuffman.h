#include "Stdafx.h"
#include "Base.h"

#pragma once

struct HTreeNode
{
	int count,parent,left,right;	//left ==-1 && right == -1 ->значит это лист дерева
	int c; //0-255 - символ, -1 - eof, -2 - escape
	//unsigned char special;	//1 - eof, 2 - escape, 0 - обычный символ
};

class AdHuffman : public Act
{
private:
	vector<HTreeNode> tree;
	int CharPosition[256];	//номера узлов с символом (-1 - значит в дереве такого символа нет)
	int SpecCharPosition[2]; //номера узлов для спецсимволов

	void _DumpTree(size_t i, bool Do);	//для отладки

	int GetStartNode(int ch);
	void AddChar(int nc);
	void UpdateChar(int nc);
	void UpdateCharLite(int nc);
	void GetCode(int ch, unsigned int &code, unsigned int &len);	//Выдает код для кодирования символа
	int NextSym(BitReader &input);	//Выдает следующий символ из закодированного потока
public:
	AdHuffman();

	virtual charbuf Do(charbuf &source) override;
	virtual charbuf UnDo(charbuf &source) override;

	void EncodeChar(int ch,OutBitStream &stream);
	int DecodeChar(BitReader &stream);
};