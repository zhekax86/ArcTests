#include "Stdafx.h"
#include "Base.h"
#include "AdHuffman.h"


#define DICTLEN 8191
//#define OFFBITS 13
//#define THRESHOLD 2

class LZB : public CLib::Base::Act
{
private:
	int FirstEntry[65536];	//указатель на первое вхождение в массиве (-1 - нет вхождений)
	int Chain[DICTLEN];	    //цепочка смещений для вхождения
	int ForwardChain[DICTLEN]; //цепочка смещений вперед

	void AddInChain(unsigned int c, size_t pos);
	void DeleteLastFromChain(unsigned int c, size_t pos);

	std::vector<HTreeNode> tree;
	std::map<unsigned int,std::pair<int,int>> treecodes;
	void CalcCodes4Tree(std::map<unsigned int, unsigned int> &stat);

public:
	virtual CLib::Base::charbuf Do(CLib::Base::charbuf &source) override;
	virtual CLib::Base::charbuf UnDo(CLib::Base::charbuf &source) override;
};