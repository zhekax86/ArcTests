#include "Stdafx.h"
#include "Base.h"
#include "LZSS.h"


class LZSSRAW : public CLib::Base::Act
{
private:
	LZSS coder;
public:
	virtual CLib::Base::charbuf Do(CLib::Base::charbuf &source) override;
	virtual CLib::Base::charbuf UnDo(CLib::Base::charbuf &source) override;
};