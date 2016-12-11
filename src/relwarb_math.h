#ifndef RELWARB_MATH_H
#define RELWARB_MATH_H

#include <math.h>

// TODO(Charly): Homemade impl ?
inline real32 Sin(real32 x) { return sinf(x); }
inline real32 Cos(real32 x) { return cosf(x); }
inline real32 Tan(real32 x) { return tanf(x); }
inline real32 ArcSin(real32 x) { return asinf(x); }
inline real32 ArcCos(real32 x) { return acosf(x); }
inline real32 ArcTan(real32 x) { return atanf(x); }

inline real32 Exp(real32 x) { return expf(x); }
inline real32 LogN(real32 x) { return logf(x); }
inline real32 Log10(real32 x)
{
    return log10f(x);
}

inline real32 Log2(real32 x)
{
    return log2f(x);
}

inline real32 Pow(real32 x, real32 y)
{
    return powf(x, y);
}


// NOTE(Charly): O(logn) pow computation for natural exponents
//               http://stackoverflow.com/a/13771544/4717805
inline real32 Pow(real32 x, uint32 n)
{
    // n = 2*d + r. x^n = (x^2)^d * x^r.
    unsigned d = n >> 1;
    unsigned r = n & 1;
    double x_2_d = d == 0 ? 1.f : Pow(x*x, d);
    double x_r = r == 0 ? 1.f : x;
    return x_2_d * x_r;
}

// NOTE(Charly): Linear version of the natural exponent pow computation
//               Can be faster than the O(logn) one on small exponents
//               due to the iterative loop
inline real32 PowL(real32 x, uint32 n)
{
    real32 result = 1.f;
    for (uint32 i = 0; i < n; ++i)
    {
        result *= x;
    }
    return result;
}

inline real32 Abs(real32 x)
{
	return (x > 0.f ? x : -x);
}

inline real32 Sqrt(real32 x)
{
    return sqrtf(x);
}

inline real32 Min(real32 x, real32 y)
{
    return x < y ? x : y;
}

inline real32 Max(real32 x, real32 y)
{
    return x > y ? x : y;
}

inline real32 Clamp(real32 x, real32 vmin, real32 vmax)
{
    return Min(vmax, Max(x, vmin));
}

// NOTE(Charly): Clamp x between 0 and 1
inline real32 Saturate(real32 x)
{
    return Clamp(x, 0.f, 1.f);
}

struct Vec2
{
    union 
    {
        struct 
        {
            real32 x;
            real32 y;
        };
        real32 data[2];
    };

    inline Vec2() {}
    inline Vec2(real32 a) { x = a; y = a; }
    inline Vec2(real32 px, real32 py) { x = px; y = py;}
    inline Vec2(const Vec2& v) = default;
    inline Vec2& operator=(const Vec2& v) = default; 

    inline Vec2& operator+=(Vec2 v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    inline Vec2& operator-=(Vec2 v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    inline Vec2& operator*=(real32 v)
    {
        x *= v;
        y *= v;
        return *this;
    }

    inline Vec2& operator/=(real32 v)
    {
        x /= v;
        y /= v;
        return *this;
    }
};

inline Vec2 operator-(Vec2 v) { return Vec2(-v.x, -v.y); }
inline Vec2 operator+(Vec2 v, Vec2 w) { return Vec2(v.x + w.x, v.y + w.y); }
inline Vec2 operator-(Vec2 v, Vec2 w) { return Vec2(v.x - w.x, v.y - w.y); }
inline Vec2 operator*(Vec2 v, real32 x) { return Vec2(v.x * x, v.y * x); }
inline Vec2 operator*(real32 x, Vec2 v) { return Vec2(v.x * x, v.y * x); }
inline Vec2 operator/(Vec2 v, real32 x) { return Vec2(v.x / x, v.y / x); }

inline real32 Dot(Vec2 v, Vec2 w) { return v.x * w.x + v.y * w.y; }
inline real32 Cross(Vec2 v, Vec2 w) { return v.x * w.y - v.y * w.x; }
inline Vec2 Times(Vec2 v, Vec2 w) { return Vec2(v.x * w.x, v.y * w.y); }
inline real32 Length(Vec2 v) { return Sqrt(Dot(v, v)); }
inline real32 LengthSquared(Vec2 v) { return Dot(v, v); }
inline Vec2 Normalize(Vec2 v) { return v / Length(v); }
inline Vec2 SafeNormalize(Vec2 v) 
{
    real32 length = Length(v);
    if (length > 1e-4f) 
    {
        return v / length;
    }

    return Vec2(0.f);
}

#endif // RELWARB_MATH_H