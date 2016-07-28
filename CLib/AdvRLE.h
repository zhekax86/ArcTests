#include "Stdafx.h"
#include "Base.h"

class AdvRLE : Act
{
public:
	virtual charbuf Do(charbuf &source) override;
	virtual charbuf UnDo(charbuf &source) override;
};