#ifndef ZMATH_HPP
#define ZMATH_HPP

#include <cmath>
#include <limits>
#include <cstdlib>

#ifndef ZMATH_NO_SSE
#	include <immintrin.h>
#endif

#ifdef ZMATH_DOUBLE_PRECISION
typedef double real;
#else
typedef float real;
#endif

#ifdef _WIN32
#pragma warning(disable: 4201) // unnamed struct
#endif

#define DEFAULT_CTORS(realype)                                                                     \
	inline realype(const realype&) = default;                                                      \
	inline realype(realype&&)      = default;                                                      \
	inline realype& operator=(const realype&) = default;                                           \
	inline realype& operator=(realype&&) = default;                                                \
	inline ~realype()                    = default

namespace z
{
constexpr real Pi       = real(3.14159265359);
constexpr real PiDiv2   = Pi / real(2);
constexpr real PiDiv4   = Pi / real(4);
constexpr real PiTimes2 = Pi * real(2);
constexpr real PiInv    = real(1) / Pi;

inline real Sqrt(real x);
inline real InvSqrt(real x);
inline real Pow(real x, int n);
inline real Floor(real x);
inline real Clamp(real x, real a, real b);
inline bool SameSign(real x, real y);
inline bool OppositeSign(real x, real y);
inline real Abs(real x);
inline real Log(real x);
inline real Log2(real x);
inline real Log10(real x);
inline real Logn(real x, real n);

inline real Cos(real x);
inline real Sin(real x);
inline real Tan(real x);
inline real ArcCos(real x);
inline real ArcSin(real x);
inline real ArcTan(real x);
inline void SinCosSquared(real x, real* psin, real* pcos);

inline void SeedRNG(unsigned int seed);
inline real GenerateRandBetween(real a = real(0), real b = real(1));
inline real GenerateRandNormal(real mean = 0.0, real stddev = 1.0);

template <typename T>
inline T Clamp(const T& x, const T& a, const T& b);
template <typename T>
inline T Saturate(const T& x);
template <typename T>
inline T Lerp(const T& a, const T& b, real t);

union vec2 {
	real data[2];
	struct
	{
		real x, y;
	};

	inline vec2& operator+=(const vec2& v);
	inline vec2& operator-=(const vec2& v);
	inline vec2& operator*=(real v);
	inline vec2& operator*=(const vec2& v); // NOTE(Charly): Component-wise mult
	inline vec2& operator/=(real v);
	inline vec2& operator/=(const vec2& v); // NOTE(Charly): Component-wise div

	/// @param i Index of vector component. Behaviour is undefined if i not in [0,Size)
	inline real& operator[](int i);
	/// @param i Index of vector component. Behaviour is undefined if i not in [0,Size)
	inline real operator[](int i) const;
};

union vec3 {
	real data[3];
	struct
	{
		real x, y, z, w;
	};

	inline vec3& operator+=(const vec3& v);
	inline vec3& operator-=(const vec3& v);
	inline vec3& operator*=(real v);
	inline vec3& operator*=(const vec3& v); // NOTE(Charly): Component-wise mult
	inline vec3& operator/=(real v);
	inline vec3& operator/=(const vec3& v); // NOTE(Charly): Component-wise div

	/// @param i Index of vector component. Behaviour is undefined if i not in [0,Size)
	inline real& operator[](int i);
	/// @param i Index of vector component. Behaviour is undefined if i not in [0,Size)
	inline real operator[](int i) const;
};

union vec4 {
	real data[4];
	struct
	{
		real x, y, z, w;
	};

	inline vec4& operator+=(const vec4& v);
	inline vec4& operator-=(const vec4& v);
	inline vec4& operator*=(real v);
	inline vec4& operator*=(const vec4& v); // NOTE(Charly): Component-wise mult
	inline vec4& operator/=(real v);
	inline vec4& operator/=(const vec4& v); // NOTE(Charly): Component-wise div

	/// @param i Index of vector component. Behaviour is undefined if i not in [0,Size)
	inline real& operator[](int i);
	/// @param i Index of vector component. Behaviour is undefined if i not in [0,Size)
	inline real operator[](int i) const;
};

inline bool operator==(const vec2& a, const vec2& b);
inline bool operator!=(const vec2& a, const vec2& b);
inline vec2 operator-(const vec2& v);
inline vec2 operator+(const vec2& v1, const vec2& v2);
inline vec2 operator-(const vec2& v1, const vec2& v2);
inline vec2 operator*(const vec2& v1, const vec2& v2);
inline vec2 operator/(const vec2& v1, const vec2& v2);
inline vec2 operator*(const vec2& v, real x);
inline vec2 operator*(real x, const vec2& v);
inline vec2 operator/(const vec2& v, real x);
inline vec2 operator/(real x, const vec2& v);

inline bool operator==(const vec3& a, const vec3& b);
inline bool operator!=(const vec3& a, const vec3& b);
inline vec3 operator-(const vec3& v);
inline vec3 operator+(const vec3& v1, const vec3& v2);
inline vec3 operator-(const vec3& v1, const vec3& v2);
inline vec3 operator*(const vec3& v1, const vec3& v2);
inline vec3 operator/(const vec3& v1, const vec3& v2);
inline vec3 operator*(const vec3& v, real x);
inline vec3 operator*(real x, const vec3& v);
inline vec3 operator/(const vec3& v, real x);
inline vec3 operator/(real x, const vec3& v);

inline bool operator==(const vec4& a, const vec4& b);
inline bool operator!=(const vec4& a, const vec4& b);
inline vec4 operator-(const vec4& v);
inline vec4 operator+(const vec4& v1, const vec4& v2);
inline vec4 operator-(const vec4& v1, const vec4& v2);
inline vec4 operator*(const vec4& v1, const vec4& v2);
inline vec4 operator/(const vec4& v1, const vec4& v2);
inline vec4 operator*(const vec4& v, real x);
inline vec4 operator*(real x, const vec4& v);
inline vec4 operator/(const vec4& v, real x);
inline vec4 operator/(real x, const vec4& v);

vec2 Vec2(real x);
vec2 Vec2(real x, real y);
vec3 Vec3(real x);
vec3 Vec3(real x, real y, real z);
vec4 Vec4(real x);
vec4 Vec4(real x, real y, real z, real w);

template <typename V>
inline real LengthSquared(const V& v);
template <typename V>
inline real Length(const V& v);
template <typename V>
inline real Dot(const V& v1, const V& v2);
template <typename V>
inline real DistSquared(const V& v1, const V& v2);
template <typename V>
inline real Dist(const V& v1, const V& v2);
template <typename V>
inline V Normalize(const V& v);
template <typename V>
inline real Cross(const V& v1, const V& v2);
template <typename V>
inline V Cross(const V& v1, const V& v2);

struct mat3
{
	vec3 data[3];

	inline mat3();
	inline explicit mat3(real x);
	inline mat3(const vec3& col0, const vec3& col1);
	inline mat3(const vec3& col0, const vec3& col1, const vec3& col2);
	inline mat3(const vec3& col0, const vec3& col1, const vec3& col2, const vec3& col3);

	DEFAULT_CTORS(mat3);

	inline mat3& operator+=(const mat3& m);
	inline mat3& operator-=(const mat3& m);
	inline mat3& operator*=(real x);
	inline mat3& operator*=(const mat3& m);
	inline mat3& operator/=(real x);
	inline mat3& operator/=(const mat3& m);

	/// @param i Index of vector component. Behaviour is undefined if i not in [0,3]
	inline vec3& operator[](int i);
	/// @param i Index of vector component. Behaviour is undefined if i not in [0,3]
	inline const vec3& operator[](int i) const;

	inline vec3 Col(int i) const;
	inline vec3 Row(int i) const;

	static inline mat3 Identity();
};

struct mat4
{
	vec4 data[4];

	inline mat4();
	inline explicit mat4(real x);
	// inline mat3(const vec3& col0, const vec3& col1);
	// inline mat3(const vec3& col0, const vec3& col1, const vec3& col2);
	// inline mat3(const vec3& col0, const vec3& col1, const vec3& col2, const vec3& col3);

	DEFAULT_CTORS(mat4);

	// inline mat3& operator+=(const mat3& m);
	// inline mat3& operator-=(const mat3& m);
	// inline mat3& operator*=(real x);
	// inline mat3& operator*=(const mat3& m);
	// inline mat3& operator/=(real x);
	// inline mat3& operator/=(const mat3& m);

	// /// @param i Index of vector component. Behaviour is undefined if i not in [0,3]
	inline vec4& operator[](int i);
	// /// @param i Index of vector component. Behaviour is undefined if i not in [0,3]
	inline const vec4& operator[](int i) const;

	inline vec4 Col(int i) const;
	inline vec4 Row(int i) const;

	// static inline mat3 Identity();
};

inline bool operator==(const mat3& a, const mat3& b);
inline mat3 operator-(const mat3& m);
inline mat3 operator+(const mat3& m1, const mat3& m2);
inline mat3 operator-(const mat3& m1, const mat3& m2);
inline mat3 operator*(const mat3& m, real x);
inline mat3 operator*(real x, const mat3& m);
inline vec3 operator*(const mat3& m, const vec3& v);
inline vec3 operator*(const vec3& v, const mat3& m);
inline mat3 operator*(const mat3& m1, const mat3& m2);
inline mat3 operator/(const mat3& m1, const mat3& m2);
inline mat3 operator/(const mat3& m, real x);
// inline mat3 Transpose(const mat3& m);
// inline real Determinant(const mat3& m);
// inline mat3 Inverse(const mat3& m);
// inline mat3 Cofactor(const mat3& m);

inline vec2 operator*(const mat3& m, const vec2& v);
inline vec2 operator*(const vec2& v, const mat3& m);

inline mat4 operator*(const mat4& m1, const mat4& m2);

inline mat3 Translation(real tx, real ty);
inline mat3 Translation(const vec2& t);
// NOTE(Charly): Rotation around "z"-axis
inline mat3 Rotation(real r);
inline mat3 Scale(real sx, real sy);
inline mat3 Scale(const vec2& s);
}

#undef DEFAULT_CTORS

namespace z
{
inline real Sqrt(real x)
{
	real result = std::sqrt(x);
	return result;
}

inline real InvSqrt(real x)
{
	// https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Approximations_that_depend_on_the_floating_point_representation
	real xhalf = 0.5f * x;
	union {
		real x;
		int  i;
	} u;
	u.x = x;
	u.i = 0x5f3759df - (u.i >> 1);
	/* The next line can be repeated any number of times to increase accuracy */
	// NOTE(Charly): Less than 3 calls does not pass the tests.
	u.x = u.x * (1.5f - xhalf * u.x * u.x);
	u.x = u.x * (1.5f - xhalf * u.x * u.x);
	u.x = u.x * (1.5f - xhalf * u.x * u.x);

	real result = u.x;

	return result;
}

inline real Pow(real x, int n)
{
	real result = 1;
	for (int i = 0; i < n; ++i)
	{
		result *= x;
	}

	return result;
}

inline real Min(real x, real y)
{
	real result = x < y ? x : y;
	return result;
}

inline real Max(real x, real y)
{
	real result = x > y ? x : y;
	return result;
}

inline real Floor(real x)
{
	real result = std::floor(x);
	return result;
}

inline real Clamp(real x, real a, real b)
{
	real result = Min(Max(x, a), b);
	return result;
}

inline real Log(real x)
{
	real result = std::log(x);
	return result;
}

inline real Log2(real x)
{
	real result = std::log2(x);
	return result;
}

inline real Log10(real x)
{
	real result = std::log10(x);
	return result;
}

inline real Logn(real x, real n)
{
	real result = Log(x) / Log(n);
	return result;
}

inline bool SameSign(real x, real y)
{
	bool result = ((Abs(x) < std::numeric_limits<real>::epsilon()) ||
	               (Abs(y) < std::numeric_limits<real>::epsilon()) ||
	               (x > real(0) && y > real(0)) || (x < real(0) && y < real(0)));
	return result;
}

inline bool OppositeSign(real x, real y)
{
	bool result = ((x > real(0) && y < real(0)) || (x < real(0) && y > real(0)));
	return result;
}

inline real Abs(real x)
{
	real result = Max(x, -x);
	return result;
}

inline real Sin(real x)
{
	return std::sin(x);
}

inline real Cos(real x)
{
	return std::cos(x);
}

inline real Tan(real x)
{
	return std::tan(x);
}

inline real ArcSin(real x)
{
	return std::asin(x);
}

inline real ArcCos(real x)
{
	return std::acos(x);
}

inline real ArcTan(real x)
{
	return std::atan(x);
}

inline void SinCosSquared(real x, real* psin, real* pcos)
{
	real s = Sin(x);
	s *= s;
	*psin = s;
	*pcos = 1 - *psin;
}

template <typename T>
inline T Clamp(const T& x, const T& a, const T& b)
{
	const real result = Max(a, Min(b, x));
	return result;
}

template <>
inline vec2 Clamp(const vec2& v, const vec2& a, const vec2& b)
{
	const real rx = Max(a.x, Min(b.x, v.x));
	const real ry = Max(a.y, Min(b.y, v.y));
	return Vec2(rx, ry);
}

template <>
inline vec3 Clamp(const vec3& v, const vec3& a, const vec3& b)
{
	const real rx = Max(a.x, Min(b.x, v.x));
	const real ry = Max(a.y, Min(b.y, v.y));
	const real rz = Max(a.z, Min(b.z, v.z));
	return Vec3(rx, ry, rz);
}

template <>
inline vec4 Clamp(const vec4& v, const vec4& a, const vec4& b)
{
	const real rx = Max(a.x, Min(b.x, v.x));
	const real ry = Max(a.y, Min(b.y, v.y));
	const real rz = Max(a.z, Min(b.z, v.z));
	const real rw = Max(a.w, Min(b.w, v.w));
	return Vec4(rx, ry, rz, rw);
}

template <typename T>
inline T Saturate(const T& x)
{
	T result = Clamp(x, T(0), T(1));
	return result;
}

template <>
inline vec2 Saturate<vec2>(const vec2& x)
{
	vec2 result = Clamp(x, Vec2(0), Vec2(1));
	return result;
}

template <>
inline vec3 Saturate<vec3>(const vec3& x)
{
	vec3 result = Clamp(x, Vec3(0), Vec3(1));
	return result;
}

template <>
inline vec4 Saturate<vec4>(const vec4& x)
{
	vec4 result = Clamp(x, Vec4(0), Vec4(1));
	return result;
}

template <typename T>
inline T Lerp(const T& a, const T& b, real t)
{
	T result = t * b + (1 - t) * a;
	return result;
}

inline void SeedRNG(unsigned int seed)
{
	std::srand(seed);
}

inline real GenerateRandBetween(real a, real b)
{
	// [0..1]
	real r = real(std::rand()) / real(RAND_MAX);
	r      = r * (b - a) + a;

	return r;
}

inline real GenerateRandNormal(real mean, real stddev)
{
	// NOTE(Charly): Box-muller method http://stackoverflow.com/a/28551411/4717805
	static real n2(0);
	static bool n2_cached = false;

	real result;

	if (!n2_cached)
	{
		real x, y, r;
		do
		{
			x = real(2) * std::rand() / real(RAND_MAX) - real(1);
			y = real(2) * std::rand() / real(RAND_MAX) - real(1);

			r = x * x + y * y;
		} while (r == real(0) || r > real(1));

		real d  = Sqrt(-2 * Log(r) / r);
		real n1 = x * d;
		n2      = y * d;

		result    = n1 * stddev + mean;
		n2_cached = true;
	}
	else
	{
		n2_cached = false;
		result    = n2 * stddev + mean;
	}

	return result;
}

inline vec2 Vec2(real x)
{
	vec2 v = {{x, x}};
	return v;
}

inline vec2 Vec2(real x, real y)
{
	vec2 v = {{x, y}};
	return v;
}

inline vec3 Vec3(real x)
{
	vec3 v = {{x, x, x}};
	return v;
}

inline vec3 Vec3(real x, real y, real z)
{
	vec3 v = {{x, y, z}};
	return v;
}

inline vec4 Vec4(real x)
{
	vec4 v = {{x, x, x, x}};
	return v;
}

inline vec4 Vec4(real x, real y, real z, real w)
{
	vec4 v = {{x, y, z, w}};
	return v;
}

inline vec2& vec2::operator+=(const vec2& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

inline vec2& vec2::operator-=(const vec2& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

inline vec2& vec2::operator*=(real a)
{
	x *= a;
	y *= a;
	return *this;
}

inline vec2& vec2::operator*=(const vec2& v)
{
	x *= v.x;
	y *= v.y;
	return *this;
}

inline vec2& vec2::operator/=(real a)
{
	x /= a;
	y /= a;
	return *this;
}

inline vec2& vec2::operator/=(const vec2& v)
{
	x /= v.x;
	y /= v.y;
	return *this;
}

inline real& vec2::operator[](int i)
{
	return data[i];
}

inline real vec2::operator[](int i) const
{
	return data[i];
}

inline bool operator==(const vec2& a, const vec2& b)
{
	bool result = (a.x == b.x) && (a.y == b.y);
	return result;
}

inline bool operator!=(const vec2& a, const vec2& b)
{
	bool result = !(a == b);
	return result;
}

inline vec2 operator-(const vec2& v)
{
	vec2 result = Vec2(-v.x, -v.y);
	return result;
}

inline vec2 operator+(const vec2& v1, const vec2& v2)
{
	vec2 result = Vec2(v1.x + v2.x, v1.y + v2.y);
	return result;
}

inline vec2 operator-(const vec2& v1, const vec2& v2)
{
	vec2 result = Vec2(v1.x - v2.x, v1.y - v2.y);
	return result;
}

inline vec2 operator*(const vec2& v1, const vec2& v2)
{
	vec2 result = Vec2(v1.x * v2.x, v1.y * v2.y);
	return result;
}

inline vec2 operator/(const vec2& v1, const vec2& v2)
{
	vec2 result = Vec2(v1.x / v2.x, v1.y / v2.y);
	return result;
}

inline vec2 operator*(const vec2& v, real x)
{
	vec2 result = Vec2(v.x * x, v.y * x);
	return result;
}

inline vec2 operator*(real x, const vec2& v)
{
	vec2 result = v * x;
	return result;
}

inline vec2 operator/(const vec2& v, real x)
{
	vec2 result = v * (real(1) / x);
	return result;
}

inline vec2 operator/(real x, const vec2& v)
{
	vec2 result = Vec2(x / v.x, x / v.y);
	return result;
}

inline real Dot(const vec2& v1, const vec2& v2)
{
	real result = v1.x * v2.x + v1.y * v2.y;
	return result;
}

inline vec3& vec3::operator+=(const vec3& v)
{
	x += v.x;
	y += v.y;
	data[2] += v.data[2];
	return *this;
}

inline vec3& vec3::operator-=(const vec3& v)
{
	x -= v.x;
	y -= v.y;
	data[2] -= v.data[2];
	return *this;
}

inline vec3& vec3::operator*=(real a)
{
	x *= a;
	y *= a;
	z *= a;
	return *this;
}

inline vec3& vec3::operator*=(const vec3& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	return *this;
}

inline vec3& vec3::operator/=(real a)
{
	x /= a;
	y /= a;
	z /= a;
	return *this;
}

inline vec3& vec3::operator/=(const vec3& v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
	return *this;
}

inline real& vec3::operator[](int i)
{
	return data[i];
}

inline real vec3::operator[](int i) const
{
	return data[i];
}

inline bool operator==(const vec3& a, const vec3& b)
{
	bool result = (a.x == b.x) && (a.y == b.y && a.z == b.z);
	return result;
}

inline bool operator!=(const vec3& a, const vec3& b)
{
	bool result = !(a == b);
	return result;
}

inline vec3 operator-(const vec3& v)
{
	vec3 result = Vec3(-v.x, -v.y, -v.z);
	return result;
}

inline vec3 operator+(const vec3& v1, const vec3& v2)
{
	vec3 result = Vec3(v1.x + v2.x, v1.y + v2.y, v1.z * v2.z);
	return result;
}

inline vec3 operator-(const vec3& v1, const vec3& v2)
{
	vec3 result = Vec3(v1.x - v2.x, v1.y - v2.y, v1.z * v2.z);
	return result;
}

inline vec3 operator*(const vec3& v1, const vec3& v2)
{
	vec3 result = Vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
	return result;
}

inline vec3 operator/(const vec3& v1, const vec3& v2)
{
	vec3 result = Vec3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
	return result;
}

inline vec3 operator*(const vec3& v, real x)
{
	vec3 result = Vec3(v.x * x, v.y * x, v.z * x);
	return result;
}

inline vec3 operator*(real x, const vec3& v)
{
	vec3 result = v * x;
	return result;
}

inline vec3 operator/(const vec3& v, real x)
{
	vec3 result = v * (real(1) / x);
	return result;
}

inline vec3 operator/(real x, const vec3& v)
{
	vec3 result = Vec3(x / v.x, x / v.y, x / v.z);
	return result;
}

inline real Dot(const vec3& v1, const vec3& v2)
{
	real result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return result;
}

inline vec4& vec4::operator+=(const vec4& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
	return *this;
}

inline vec4& vec4::operator-=(const vec4& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
	return *this;
}

inline vec4& vec4::operator*=(real a)
{
	x *= a;
	y *= a;
	z *= a;
	w *= a;
	return *this;
}

inline vec4& vec4::operator*=(const vec4& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	w *= v.w;
	return *this;
}

inline vec4& vec4::operator/=(real a)
{
	x /= a;
	y /= a;
	z /= a;
	y /= a;
	return *this;
}

inline vec4& vec4::operator/=(const vec4& v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
	w /= v.w;
	return *this;
}

inline real& vec4::operator[](int i)
{
	return data[i];
}

inline real vec4::operator[](int i) const
{
	return data[i];
}

inline bool operator==(const vec4& a, const vec4& b)
{
	bool result = (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);
	return result;
}

inline bool operator!=(const vec4& a, const vec4& b)
{
	bool result = !(a == b);
	return result;
}

inline vec4 operator-(const vec4& v)
{
	vec4 result = Vec4(-v.x, -v.y, -v.z, -v.w);
	return result;
}

inline vec4 operator+(const vec4& v1, const vec4& v2)
{
	vec4 result = Vec4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
	return result;
}

inline vec4 operator-(const vec4& v1, const vec4& v2)
{
	vec4 result = Vec4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
	return result;
}

inline vec4 operator*(const vec4& v1, const vec4& v2)
{
	vec4 result = Vec4(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
	return result;
}

inline vec4 operator/(const vec4& v1, const vec4& v2)
{
	vec4 result = Vec4(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
	return result;
}

inline vec4 operator*(const vec4& v, real x)
{
	vec4 result = Vec4(v.x * x, v.y * x, v.z * x, v.w * x);
	return result;
}

inline vec4 operator*(real x, const vec4& v)
{
	vec4 result = v * x;
	return result;
}

inline vec4 operator/(const vec4& v, real x)
{
	vec4 result = v * (real(1) / x);
	return result;
}

inline vec4 operator/(real x, const vec4& v)
{
	vec4 result = Vec4(x / v.x, x / v.y, x / v.z, x / v.w);
	return result;
}

inline real Dot(const vec4& v1, const vec4& v2)
{
	real result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
	return result;
}

template <typename V>
inline real LengthSquared(const V& v)
{
	real result = Dot(v, v);
	return result;
}

template <typename V>
inline real Length(const V& v)
{
	real result = Sqrt(LengthSquared(v));
	return result;
}

template <typename V>
inline real DistSquared(const V& v1, const V& v2)
{
	real result = LengthSquared(v2 - v1);
	return result;
}

template <typename V>
inline real Dist(const V& v1, const V& v2)
{
	real result = Length(v2 - v1);
	return result;
}

template <typename V>
inline V Normalize(const V& v)
{
	V result = v * InvSqrt(LengthSquared(v));
	return result;
}

inline real Cross(const vec2& v1, const vec2& v2)
{
	real result = v1.x * v2.y - v1.y * v2.x;
	return result;
}

inline vec3 Cross(const vec3& v1, const vec3& v2)
{
	vec3 result;
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;

	return result;
}

inline mat3::mat3() {}

inline mat3::mat3(real x)
{
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			data[i][j] = (i == j ? x : 0);
		}
	}
}

inline mat3::mat3(const vec3& row0, const vec3& row1, const vec3& row2)
{
	data[0] = row0;
	data[1] = row1;
	data[2] = row2;
}

inline mat3& mat3::operator+=(const mat3& m)
{
	for (int i = 0; i < 3 * 3; ++i)
	{
		data[i] += m.data[i];
	}

	return *this;
}

inline mat3& mat3::operator-=(const mat3& m)
{
	for (int i = 0; i < 3 * 3; ++i)
	{
		data[i] -= m.data[i];
	}

	return *this;
}

inline mat3& mat3::operator*=(real x)
{
	for (int i = 0; i < 3 * 3; ++i)
	{
		data[i] *= x;
	}

	return *this;
}

inline mat3& mat3::operator/=(real x)
{
	for (int i = 0; i < 3 * 3; ++i)
	{
		data[i] /= x;
	}

	return *this;
}

inline mat3& mat3::operator*=(const mat3& m)
{
	*this = (*this) * m;
	return *this;
}

// inline mat3& mat3::operator/=(const mat3& m)
// {
//     (*this) *= Inverse(m);
//     return *this;
// }

inline vec3& mat3::operator[](int index)
{
	return data[index];
}

inline const vec3& mat3::operator[](int index) const
{
	return data[index];
}

inline vec3 mat3::Row(int index) const
{
	vec3 result = data[index];
	return result;
}

inline vec3 mat3::Col(int index) const
{
	vec3 result;

	for (int i = 0; i < 3; ++i)
	{
		result[i] = data[i][index];
	}

	return result;
}

inline mat3 mat3::Identity()
{
	mat3 result(1.0);
	return result;
}

inline bool operator==(const mat3& a, const mat3& b)
{
	bool result = true;

	for (int i = 0; i < 3; ++i)
	{
		if (a[i] != b[i])
		{
			result = false;
			break;
		}
	}

	return result;
}

inline mat3 operator-(const mat3& m)
{
	mat3 result;

	for (int i = 0; i < 3 * 3; ++i)
	{
		result.data[i] = -m.data[i];
	}

	return result;
}

inline mat3 operator+(const mat3& m1, const mat3& m2)
{
	mat3 result;

	for (int i = 0; i < 3 * 3; ++i)
	{
		result.data[i] = m1.data[i] + m2.data[i];
	}

	return result;
}

inline mat3 operator-(const mat3& m1, const mat3& m2)
{
	mat3 result;

	for (int i = 0; i < 3 * 3; ++i)
	{
		result.data[i] = m1.data[i] - m2.data[i];
	}

	return result;
}

inline mat3 operator*(const mat3& m, real x)
{
	mat3 result;

	for (int i = 0; i < 3 * 3; ++i)
	{
		result.data[i] = m.data[i] * x;
	}

	return result;
}

inline mat3 operator*(real x, const mat3& m)
{
	mat3 result;

	for (int i = 0; i < 3 * 3; ++i)
	{
		result.data[i] = x * m.data[i];
	}

	return result;
}

inline vec3 operator*(const mat3& m, const vec3& v)
{
	vec3 result;

	for (int i = 0; i < 3; ++i)
	{
		result[i] = Dot(m.Row(i), v);
	}

	return result;
}

inline vec3 operator*(const vec3& v, const mat3& m)
{
	vec3 result;

	for (int i = 0; i < 3; ++i)
	{
		result[i] = Dot(v, m.Col(i));
	}

	return result;
}

inline mat3 operator*(const mat3& m1, const mat3& m2)
{
	mat3 result;

	for (int row = 0; row < 3; ++row)
	{
		for (int col = 0; col < 3; ++col)
		{
			real prod        = Dot(m1.Row(row), m2.Col(col));
			result[row][col] = prod;
		}
	}

	return result;
}

inline mat3 operator/(const mat3& m, real x)
{
	mat3 result;

	for (int i = 0; i < 3; ++i)
	{
		result.data[i] = m.data[i] / x;
	}

	return result;
}

inline mat3 Transpose(const mat3& m)
{
	mat3 result;

	for (int row = 0; row < 3; ++row)
	{
		for (int col = 0; col < 3; ++col)
		{
			result[col][row] = m[row][col];
		}
	}

	return result;
}

// inline real Determinant(const mat3& m)
// {
//     const real a = m[0][0] * m[1][1] * m[2][2];
//     const real b = m[0][1] * m[1][2] * m[2][0];
//     const real c = m[0][2] * m[1][0] * m[2][1];
//     const real d = m[0][1] * m[1][0] * m[2][2];
//     const real e = m[0][0] * m[1][2] * m[2][1];
//     const real f = m[0][2] * m[1][1] * m[2][0];

//     real result = a + b + c - d - e - f;
//     return result;
// }

// inline mat3 Inverse(const mat3& m)
// {
//     mat3 result;
//     // TODO
//     return result;
// }

inline mat3 OuterProduct(const vec3& u, const vec3& v)
{
	mat3 result;

	for (int row = 0; row < 3; ++row)
	{
		for (int col = 0; col < 3; ++col)
		{
			result[row][col] = u[row] * v[col];
		}
	}

	return result;
}

inline vec2 operator*(const mat3& m, const vec2& v)
{
	vec3 v1 = Vec3(v.x, v.y, real(1));

	v1 = m * v1;
	v1 /= v1.z;

	vec2 result = Vec2(v1.x, v1.y);
	return result;
}

inline vec2 operator*(const vec2& v, const mat3& m)
{
	vec3 v1 = Vec3(v.x, v.y, real(1));

	v1 = v1 * m;
	v1 /= v1.z;

	vec2 result = Vec2(v1.x, v1.y);
	return result;
}

inline mat4::mat4() {}

inline mat4::mat4(real x)
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			data[i][j] = (i == j ? x : 0);
		}
	}
}

inline vec4& mat4::operator[](int index)
{
	return data[index];
}

inline const vec4& mat4::operator[](int index) const
{
	return data[index];
}

inline mat4 operator*(const mat4& m1, const mat4& m2)
{
	mat4 result;

	for (int row = 0; row < 4; ++row)
	{
		for (int col = 0; col < 4; ++col)
		{
			real prod        = Dot(m1.Row(row), m2.Col(col));
			result[row][col] = prod;
		}
	}

	return result;
}

inline vec4 mat4::Row(int index) const
{
	vec4 result = data[index];
	return result;
}

inline vec4 mat4::Col(int index) const
{
	vec4 result;

	for (int i = 0; i < 4; ++i)
	{
		result[i] = data[i][index];
	}

	return result;
}

inline mat3 CrossMatrix(const vec3& u)
{
	mat3 result(0);

	result[0][1] = -u.z;
	result[0][2] = u.y;
	result[1][0] = u.z;
	result[1][2] = -u.x;
	result[2][0] = -u.y;
	result[2][1] = u.x;

	return result;
}

inline mat4 LookAt(const vec3& eye, const vec3& target, const vec3& up)
{
	mat4 m;
	vec3 f = Normalize(target - eye);
	vec3 u = Normalize(up);
	vec3 s = Normalize(Cross(f, u));
	u      = Cross(s, f);
	f      = -f;

	mat4 t(1);
	t[0][3] = -eye.x;
	t[1][3] = -eye.y;
	t[2][3] = -eye.z;

	mat4 result = m * t;
	return result;
}

inline mat4 Ortho(real left, real right, real top, real bottom)
{
	mat4 result(1);

	result[0][0] = real(2) / (right - left);
	result[1][1] = real(2) / (bottom - top);
	result[0][3] = -(right + left) / (right - left);
	result[1][3] = -(top + bottom) / (top - bottom);

	return result;
}

inline mat3 Translation(real tx, real ty)
{
	mat3 result(1);
	result[0][2] = tx;
	result[1][2] = ty;

	return result;
}

inline mat3 Translation(const vec2& t)
{
	mat3 result = Translation(t.x, t.y);
	return result;
}

inline mat3 Rotation(real r)
{
	mat3 result(1);

	const real c = Cos(r);
	const real s = Sin(r);

	result[0][0] = c;
	result[0][1] = -s;
	result[1][0] = s;
	result[1][1] = c;

	return result;
}

inline mat3 Scale(real sx, real sy)
{
	mat3 result(1);
	result[0][0] = sx;
	result[1][1] = sy;

	return result;
}

inline mat3 Scale(const vec2& s)
{
	mat3 result = Scale(s.x, s.y);
	return result;
}
}

#endif // ZMATH_HPP
