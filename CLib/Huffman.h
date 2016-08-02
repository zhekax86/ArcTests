#include "Stdafx.h"
#include "Base.h"
#include "AdHuffman.h"

class Huffman : public Act
{
private:
	vector<HTreeNode> tree;
	unsigned int codes[256];	 //Коды символов
	unsigned char codelen[256];  //Длина кода

	void MakeCodes(charbuf &source);
public:
	virtual charbuf Do(charbuf &source) override;
	virtual charbuf UnDo(charbuf &source) override;
};