#include "Stdafx.h"
#include "Base.h"


class Probability : public Act
{
private:
	unsigned char PossibleChar[65536];
public:
	Probability();

	virtual charbuf Do(charbuf &source) override;
	virtual charbuf UnDo(charbuf &source) override;
};