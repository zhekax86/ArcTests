#include "Stdafx.h"
#include "Base.h"
#include "AdHuffman.h"

class Huffman : public CLib::Base::Act
{
private:
	std::vector<HTreeNode> tree;
	unsigned int codes[256];	 //Коды символов
	unsigned char codelen[256];  //Длина кода

	void MakeCodes(CLib::Base::charbuf &source);
public:
	virtual CLib::Base::charbuf Do(CLib::Base::charbuf &source) override;
	virtual CLib::Base::charbuf UnDo(CLib::Base::charbuf &source) override;
};