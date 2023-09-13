#include "stdafx.h"
#include "Act.h"

using namespace std;

namespace CLib::Base
{
	charbuf Act::Do(charbuf& source)
	{
		return move(source);
		//return unique_ptr<charbuf>(new charbuf(*source));
	}

	charbuf Act::UnDo(charbuf& source)
	{
		return move(source);
		//return unique_ptr<charbuf>(new charbuf(*source));
	}
}