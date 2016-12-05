
#include "relwarb_utils.h"

real32 Abs(real32 x)
{
	return ( x > 0.f ? x : -x );
}

// cf. http://www.gamasutra.com/view/feature/131790/simple_intersection_tests_for_games.php?page=3
bool32 Intersect(const RectangularShape & shape1, const RectangularShape & shape2)
{
	real32 diffX = Abs(shape1.posX - shape2.posX);
	real32 diffY = Abs(shape1.posY - shape2.posY);

	return ( diffX <= shape1.sizeX + shape2.sizeX && diffY <= shape1.sizeY + shape2.sizeY );
}

bool32 StrEqual(const char* str1, const char* str2, int length)
{
	bool32 matched = true;

	for (int idx = 0; idx < length; ++idx)
	{
		if (*str1 != *str2)
		{
			matched = false;
			break;
		}

		++str1;
		++str2;
	}

	return matched;
}

uint32 StrLength(const char* str)
{
	uint32 result = 0;
	for (; *str; ++str, ++result);

	return result;
}