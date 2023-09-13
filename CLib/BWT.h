#pragma once
#include "Stdafx.h"
#include "Base.h"

//Преобразование Барроуза Уилера
// https://ru.wikipedia.org/wiki/%D0%9F%D1%80%D0%B5%D0%BE%D0%B1%D1%80%D0%B0%D0%B7%D0%BE%D0%B2%D0%B0%D0%BD%D0%B8%D0%B5_%D0%91%D0%B0%D1%80%D1%80%D0%BE%D1%83%D0%B7%D0%B0_%E2%80%94_%D0%A3%D0%B8%D0%BB%D0%B5%D1%80%D0%B0

class BWT : public CLib::Base::Act
{
protected:
	CLib::Base::charbuf EncodeBuf(CLib::Base::charbuf &source, std::vector<int> &SortedStrings);
	CLib::Base::charbuf DecodeBuf(CLib::Base::charbuf &source, std::vector<int> &Order);
public:
	virtual CLib::Base::charbuf Do(CLib::Base::charbuf &source) override;
	virtual CLib::Base::charbuf UnDo(CLib::Base::charbuf &source) override;
};