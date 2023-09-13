#include "Stdafx.h"
#include "Base.h"
#include "LZSS.h"
#include "AdHuffman.h"


class LZSSAdHuf : public CLib::Base::Act
{
private:
	LZSS coder;
public:
	virtual CLib::Base::charbuf Do(CLib::Base::charbuf &source) override;
	virtual CLib::Base::charbuf UnDo(CLib::Base::charbuf &source) override;
};