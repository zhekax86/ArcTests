#include "Stdafx.h"
#include "Base.h"
#include "Base\Act.h"
#include "Base\Charbuf.h"

class RLE12 : public CLib::Base::Act
{
public:
	virtual CLib::Base::charbuf Do(CLib::Base::charbuf &source) override;
	virtual CLib::Base::charbuf UnDo(CLib::Base::charbuf &source) override;
};