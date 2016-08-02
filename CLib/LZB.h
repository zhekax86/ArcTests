#include "Stdafx.h"
#include "Base.h"
#include "AdHuffman.h"


#define DICTLEN 8191
//#define OFFBITS 13
//#define THRESHOLD 2

class LZB : public Act
{
private:
	int FirstEntry[65536];	//указатель на первое вхождение в массиве (-1 - нет вхождений)
	int Chain[DICTLEN];	    //цепочка смещений для вхождения
	int ForwardChain[DICTLEN]; //цепочка смещений вперед

	void AddInChain(unsigned int c, size_t pos);
	void DeleteLastFromChain(unsigned int c, size_t pos);

	vector<HTreeNode> tree;
	map<uint,pair<int,int>> treecodes;
	void CalcCodes4Tree(map<uint,uint> &stat);
public:
	//Probability();

	virtual charbuf Do(charbuf &source) override;
	virtual charbuf UnDo(charbuf &source) override;
};