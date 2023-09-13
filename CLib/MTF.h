#include "Stdafx.h"
#include "Base.h"
#include "Act.h"

class MTF : public CLib::Base::Act
{
private:
	unsigned char symbol[256];
	inline unsigned char GetSymbol(unsigned char c);
	inline unsigned char GetSymbolBackwards(unsigned char c);
public:
	MTF();

	virtual CLib::Base::charbuf Do(CLib::Base::charbuf &source) override;
	virtual CLib::Base::charbuf UnDo(CLib::Base::charbuf &source) override;
};