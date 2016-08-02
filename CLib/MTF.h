#include "Stdafx.h"
#include "Base.h"

class MTF : public Act
{
private:
	unsigned char symbol[256];
	inline unsigned char GetSymbol(unsigned char c);
	inline unsigned char GetSymbolBackwards(unsigned char c);
public:
	MTF();

	virtual charbuf Do(charbuf &source) override;
	virtual charbuf UnDo(charbuf &source) override;
};