#include "Stdafx.h"
#include "Base.h"


class Probability : public CLib::Base::Act
{
private:
	unsigned char PossibleChar[65536];
public:
	Probability();

	virtual CLib::Base::charbuf Do(CLib::Base::charbuf &source) override;
	virtual CLib::Base::charbuf UnDo(CLib::Base::charbuf &source) override;
};