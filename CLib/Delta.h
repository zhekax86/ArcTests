#include "Stdafx.h"
#include "Base.h"

class Delta : public Act
{
public:
	virtual charbuf Do(charbuf &source) override;
	virtual charbuf UnDo(charbuf &source) override;
};