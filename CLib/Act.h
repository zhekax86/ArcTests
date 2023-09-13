#pragma once

#include "Charbuf.h"

namespace CLib::Base
{
	//Базовый класс для какой-либо обработки charbuf
	//Все алгоритмы наследуются от него
	class Act
	{
	public:
		virtual charbuf Do(charbuf& source);
		virtual charbuf UnDo(charbuf& source);
	};
}
