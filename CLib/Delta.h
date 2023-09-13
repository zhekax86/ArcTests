#include "Stdafx.h"
#include "Base.h"
#include "Act.h"

class Delta : public CLib::Base::Act
{
public:
	virtual CLib::Base::charbuf Do(CLib::Base::charbuf &source) override;
	virtual CLib::Base::charbuf UnDo(CLib::Base::charbuf &source) override;
};