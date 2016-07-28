#include "Stdafx.h"
#include "Base.h"
#include "LZSS.h"


class LZSSRAW : public Act
{
private:
	LZSS coder;
public:
	virtual charbuf Do(charbuf &source) override;
	virtual charbuf UnDo(charbuf &source) override;
};