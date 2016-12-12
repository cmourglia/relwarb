
#include "relwarb_utils.h"
#include "relwarb_math.h"

// cf. http://www.gamasutra.com/view/feature/131790/simple_intersection_tests_for_games.php?page=3
bool32 Intersect(Vec2 pos1, const RectangularShape * shape1, Vec2 pos2, const RectangularShape * shape2)
{
	real32 diffX = Abs(pos1.x + shape1->offset.x - pos2.x - shape2->offset.x);
	real32 diffY = Abs(pos1.y + shape1->offset.y - pos2.y - shape2->offset.y);
	return ( diffX <= (shape1->size.x + shape2->size.x) * 0.5f && diffY <= (shape1->size.y + shape2->size.y) * 0.5f );
}

Vec2 Overlap(Vec2 pos1, const RectangularShape * shape1, Vec2 pos2, const RectangularShape * shape2)
{
	Vec2 over( pos1.x + shape1->offset.x - pos2.x - shape2->offset.x,
			   pos1.y + shape1->offset.y - pos2.y - shape2->offset.y);
	Vec2 size(shape1->size + shape2->size);
	Vec2 sign(over.x > 0.f ? 1.f : -1.f, over.y > 0.f ? 1.f : -1.f);
	return over - 0.5f * Times(sign, size);
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
