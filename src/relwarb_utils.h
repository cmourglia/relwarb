#ifndef RELWARB_UTILS_H
#define RELWARB_UTILS_H

#include "relwarb_defines.h"
#include "relwarb.h"

bool32 Intersect(Vec2 pos1, const Shape* shape1, Vec2 pos2, const Shape* shape2);

// Returns the overlap w.r.t shape1 so that applying the opposite vector to shape1 would lead to shapes being just in contact.
Vec2 Overlap(Vec2 pos1, const Shape* shape1, Vec2 pos2, const Shape* shape2);

// NOTE(Charly): Checks for equality between str1 and str2
//               Behaviour is undefined is one of the strings' length is less that length.
bool32 StrEqual(const char* str1, const char* str2, int length);

uint32 StrLength(const char* str);

#endif // RELWARB_UTILS_H