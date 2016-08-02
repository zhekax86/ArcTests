#include "Stdafx.h"
#include "Base.h"

class RLE12 : public Act
{
public:
	virtual charbuf Do(charbuf &source) override;
	virtual charbuf UnDo(charbuf &source) override;
};