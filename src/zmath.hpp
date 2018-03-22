#ifndef ZMATH_HPP
#define ZMATH_HPP

#include <cmath>
#include <limits>
#include <cstdlib>

#ifndef ZMATH_NO_SSE
#include <immintrin.h>
#endif

#ifdef ZMATH_DOUBLE_PRECISION
typedef double real;
#else
typedef float real;
#endif

#define DEFAULT_CTORS(realype)                              \
    inline realype(const realype&) = default;               \
    inline realype(realype&&) = default;                    \
    inline realype& operator=(const realype&) = default;    \
    inline realype& operator=(realype&&) = default;         \
    inline ~realype() = default

namespace z
{
    constexpr real Pi = real(3.14159265359);
    constexpr real PiDiv2 = Pi / real(2);
    constexpr real PiDiv4 = Pi / real(4);
    constexpr real PiTimes2 = Pi * real(2);
    constexpr real PiInv = real(1) / Pi;

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
    inline real GenerateRandNormal(real mean = 0.0,  real stddev = 1.0);

    template <typename T> inline T Clamp(const T& x, const T& a, const T& b);
    template <typename T> inline T Saturate(const T& x);
    template <typename T> inline T Lerp(const T& a, const T& b, real t);

    template <int Size>
    struct vec
    {
        real data[Size];

        inline vec& operator+=(const vec& v);
        inline vec& operator-=(const vec& v);
        inline vec& operator*=(real v);
        inline vec& operator*=(const vec& v); // NOTE(Charly): Component-wise mult
        inline vec& operator/=(real v);
        inline vec& operator/=(const vec& v); // NOTE(Charly): Component-wise div

        /// @param i Index of vector component. Behaviour is undefined if i not in [0,Size)
        inline real& operator[](int i);
        /// @param i Index of vector component. Behaviour is undefined if i not in [0,Size)
        inline const real& operator[](int i) const;

        inline real& x();
        inline const real& x() const;

        inline real& y();
        inline const real& y() const;

        inline real& z();
        inline const real& z() const;

        inline real& w();
        inline const real& w() const;
    };

    template <int Size> inline bool operator==(const vec<Size>& a, const vec<Size>& b);
    template <int Size> inline bool operator>(const vec<Size>& a, const vec<Size>& b);
    template <int Size> inline bool operator<(const vec<Size>& a, const vec<Size>& b);
    template <int Size> inline bool operator<=(const vec<Size>& a, const vec<Size>& b);
    template <int Size> inline bool operator>=(const vec<Size>& a, const vec<Size>& b);
    template <int Size> inline vec<Size> operator-(const vec<Size>& v);
    template <int Size> inline vec<Size> operator+(const vec<Size>& v1, const vec<Size>& v2);
    template <int Size> inline vec<Size> operator-(const vec<Size>& v1, const vec<Size>& v2);
    template <int Size> inline vec<Size> operator*(const vec<Size>& v1, const vec<Size>& v2);
    template <int Size> inline vec<Size> operator/(const vec<Size>& v1, const vec<Size>& v2);
    template <int Size> inline vec<Size> operator*(const vec<Size>& v, real x);
    template <int Size> inline vec<Size> operator*(real x, const vec<Size>& v);
    template <int Size> inline vec<Size> operator/(const vec<Size>& v, real x);
    template <int Size> inline vec<Size> operator/(real x, const vec<Size>& v);
    template <int Size> inline real LengthSquared(const vec<Size>& v);
    template <int Size> inline real Length(const vec<Size>& v);
    template <int Size> inline real Dot(const vec<Size>& v1, const vec<Size>& v2);
    template <int Size> inline real DistSquared(const vec<Size>& v1, const vec<Size>& v2);
    template <int Size> inline real Dist(const vec<Size>& v1, const vec<Size>& v2);
    template <int Size> inline vec<Size> Normalize(const vec<Size>& v);

    vec<2> Vec2(real x);
    vec<2> Vec2(real x, real y);
    vec<3> Vec3(real x);
    vec<3> Vec3(real x, real y, real z);
    vec<4> Vec4(real x);
    vec<4> Vec4(real x, real y, real z, real w);

    inline real Cross(const vec<2>& v1, const vec<2>& v2);
    inline vec<3> Cross(const vec<3>& v1, const vec<3>& v2);

    template <int Size>
    union mat
    {
        typedef vec<Size> V;
        V rows[Size];
        real data[Size * Size];

        inline mat();
        inline explicit mat(real x);
        inline mat(const V& col0, const V& col1);
        inline mat(const V& col0, const V& col1, const V& col2);
        inline mat(const V& col0, const V& col1, const V& col2, const V& col3);

        DEFAULT_CTORS(mat);

        inline mat& operator+=(const mat& m);
        inline mat& operator-=(const mat& m);
        inline mat& operator*=(real x);
        inline mat& operator*=(const mat& m);
        inline mat& operator/=(real x);
        inline mat& operator/=(const mat& m);

        /// @param i Index of vector component. Behaviour is undefined if i not in [0,3]
        inline V& operator[](int i);
        /// @param i Index of vector component. Behaviour is undefined if i not in [0,3]
        inline const V& operator[](int i) const;

        inline V Col(int i) const;
        inline V Row(int i) const;

        static inline mat Identity();
    };

    template <int Size> inline bool operator==(const mat<Size>& a, const mat<Size>& b);
    template <int Size> inline mat<Size> operator-(const mat<Size>& m);
    template <int Size> inline mat<Size> operator+(const mat<Size>& m1, const mat<Size>& m2);
    template <int Size> inline mat<Size> operator-(const mat<Size>& m1, const mat<Size>& m2);
    template <int Size> inline mat<Size> operator*(const mat<Size>& m, real x);
    template <int Size> inline mat<Size> operator*(real x, const mat<Size>& m);
    template <int Size> inline vec<Size> operator*(const mat<Size>& m, const vec<Size>& v);
    template <int Size> inline vec<Size> operator*(const vec<Size>& v, const mat<Size>& m);
    template <int Size> inline mat<Size> operator*(const mat<Size>& m1, const mat<Size>& m2);
    template <int Size> inline mat<Size> operator/(const mat<Size>& m1, const mat<Size>& m2);
    template <int Size> inline mat<Size> operator/(const mat<Size>& m, real x);
    template <int Size> inline mat<Size> Transpose(const mat<Size>& m);
    template <int Size> inline real Determinant(const mat<Size>& m);
    template <int Size> inline mat<Size> Inverse(const mat<Size>& m);
    template <int Size> inline mat<Size> Cofactor(const mat<Size>& m);

    template <int Size> inline vec<Size-1> operator*(const mat<Size>& m, const vec<Size-1>& v);
    template <int Size> inline vec<Size-1> operator*(const vec<Size-1>& v, const mat<Size>& m);

    typedef vec<2> vec2;
    typedef vec<3> vec3;
    typedef vec<4> vec4;

    typedef mat<2> mat2;
    typedef mat<3> mat3;
    typedef mat<4> mat4;

    inline mat4 LookAt(const vec3& eye, const vec3& target, const vec3& up);
    inline mat4 Ortho(real left, real right, real top, real bottom);

    inline mat3 Translation(real tx, real ty);
    inline mat3 Translation(const vec2& t);
    // NOTE(Charly): Rotation around "z"-axis
    inline mat3 Rotation(real r);
    inline mat3 Scale(real sx, real sy);
    inline mat3 Scale(const vec2& s);

    inline mat4 Translation(real tx, real ty, real tz);
    inline mat4 Translation(const vec3& t);
    inline mat4 Rotation(real rx, real ry, real rz);
    inline mat4 Rotation(const vec3& r);
    inline mat4 Rotation(const vec3& axis, real angle);
    inline mat4 Scale(real sx, real sy, real sz);
    inline mat4 Scale(const vec3& s);
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
        union
        {
            real x;
            int i;
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
                       (x > real(0) && y > real(0)) ||
                       (x < real(0) && y < real(0)));
        return result;
    }

    inline bool OppositeSign(real x, real y)
    {
        bool result = ((x > real(0) && y < real(0)) ||
                       (x < real(0) && y > real(0)));
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

    template <typename T> inline T Clamp(const T& x, const T& a, const T& b)
    {
        T result;
        if (x <= a)
        {
            result = a;
        }
        else if (result >= b)
        {
            result = b;
        }
        else
        {
            result = x;
        }

        return result;
    }

    template <typename T> inline T Saturate(const T& x)
    {
        T result = Clamp(x, T(0), T(1));
        return result;
    }

    template <> inline vec2 Saturate<vec2>(const vec2& x)
    {
        vec2 result = Clamp(x, Vec2(0), Vec2(1));
        return result;
    }

    template <> inline vec3 Saturate<vec3>(const vec3& x)
    {
        vec3 result = Clamp(x, Vec3(0), Vec3(1));
        return result;
    }

    template <> inline vec4 Saturate<vec4>(const vec4& x)
    {
        vec4 result = Clamp(x, Vec4(0), Vec4(1));
        return result;
    }

    template <typename T> inline T Lerp(const T& a, const T& b, real t)
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
        r = r * (b - a) + a;

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
            }
            while (r == real(0) || r > real(1));

            real d = Sqrt(-2 * Log(r) / r);
            real n1 = x * d;
            n2 = y * d;

            result = n1 * stddev + mean;
            n2_cached = true;
        }
        else
        {
            n2_cached = false;
            result = n2 * stddev + mean;
        }

        return result;
    }

    inline vec<2> Vec2(real x) { return vec<2>{x, x}; }
    inline vec<2> Vec2(real x, real y) { return vec<2>{x, y}; }
    inline vec<3> Vec3(real x) { return vec<3>{x, x, x}; }
    inline vec<3> Vec3(real x, real y, real z) { return vec<3>{x, y, z}; }
    inline vec<4> Vec4(real x) { return vec<4>{x, x, x, x}; }
    inline vec<4> Vec4(real x, real y, real z, real w) { return vec<4>{x, y, z, w}; }

    template <int Size> inline vec<Size>& vec<Size>::operator+=(const vec<Size>& v)
    {
        for (int i = 0; i < Size; ++i)
        {
            data[i] += v.data[i];
        }
        return *this;
    }

    template <int Size> inline vec<Size>& vec<Size>::operator-=(const vec<Size>& v)
    {
        for (int i = 0; i < Size; ++i)
        {
            data[i] -= v.data[i];
        }
        return *this;
    }

    template <int Size> inline vec<Size>& vec<Size>::operator*=(real x)
    {
        for (int i = 0; i < Size; ++i)
        {
            data[i] *= x;
        }
        return *this;
    }

    template <int Size> inline vec<Size>& vec<Size>::operator*=(const vec<Size>& v)
    {
        for (int i = 0; i < Size; ++i)
        {
            data[i] *= v.data[i];
        }
        return *this;
    }

    template <int Size> inline vec<Size>& vec<Size>::operator/=(real x)
    {
        for (int i = 0; i < Size; ++i)
        {
            data[i] /= x;
        }
        return *this;
    }

    template <int Size> inline vec<Size>& vec<Size>::operator/=(const vec<Size>& v)
    {
        for (int i = 0; i < Size; ++i)
        {
            data[i] /= v.data[i];
        }
        return *this;
    }

    template <int Size> inline real& vec<Size>::operator[](int i)
    {
        return data[i];
    }

    template <int Size> inline const real& vec<Size>::operator[](int i) const
    {
        return data[i];
    }

    template <int Size> inline real& vec<Size>::x()
    {
        static_assert(Size == 2 || Size == 3 || Size == 4, "vec::x() is only valid for vectors of size 2, 3 or 4.");
        return data[0];
    }

    template <int Size> inline const real& vec<Size>::x() const
    {
        static_assert(Size == 2 || Size == 3 || Size == 4, "vec::x() is only valid for vectors of size 2, 3 or 4.");
        return data[0];
    }

    template <int Size> inline real& vec<Size>::y()
    {
        static_assert(Size == 2 || Size == 3 || Size == 4, "vec::y() is only valid for vectors of size 2, 3 or 4.");
        return data[1];
    }

    template <int Size> inline const real& vec<Size>::y() const
    {
        static_assert(Size == 2 || Size == 3 || Size == 4, "vec::y() is only valid for vectors of size 2, 3 or 4.");
        return data[1];
    }

    template <int Size> inline real& vec<Size>::z()
    {
        static_assert(Size == 3 || Size == 4, "vec::z() is only valid for vectors of size 3 or 4.");
        return data[2];
    }

    template <int Size> inline const real& vec<Size>::z() const
    {
        static_assert(Size == 3 || Size == 4, "vec::z() is only valid for vectors of size 3 or 4.");
        return data[2];
    }

    template <int Size> inline real& vec<Size>::w()
    {
        static_assert(Size == 4, "vec::w() is only valid for vectors of size 4.");
        return data[3];
    }

    template <int Size> inline const real& vec<Size>::w() const
    {
        static_assert(Size == 4, "vec::w() is only valid for vectors of size 4.");
        return data[3];
    }

    template <int Size> inline bool operator==(const vec<Size>& a, const vec<Size>& b)
    {
        bool result = true;

        for (int i = 0; i < Size; ++i)
        {
            if (a[i] != b[i])
            {
                result = false;
                break;
            }
        }

        return result;
    }

    template <int Size> inline bool operator<(const vec<Size>& a, const vec<Size>& b)
    {
        bool result = true;

        for (int i = 0; i < Size; ++i)
        {
            if (a[i] > b[i])
            {
                result = false;
                break;
            }
        }

        return result;
    }

    template <int Size> inline bool operator>(const vec<Size>& a, const vec<Size>& b)
    {
        bool result = true;

        for (int i = 0; i < Size; ++i)
        {
            if (a[i] < b[i])
            {
                result = false;
                break;
            }
        }

        return result;
    }

    template <int Size> inline bool operator<=(const vec<Size>& a, const vec<Size>& b)
    {
       bool result = !(a > b);
       return result;
    }

    template <int Size> inline bool operator>=(const vec<Size>& a, const vec<Size>& b)
    {
        bool result = !(a < b);
        return result;
    }

    template <int Size> inline vec<Size> operator-(const vec<Size>& v)
    {
        vec<Size> result;

        for (int i = 0; i < Size; ++i)
        {
            result.data[i] = -v.data[i];
        }

        return result;
    }

    template <int Size> inline vec<Size> operator+(const vec<Size>& v1, const vec<Size>& v2)
    {
        vec<Size> result;

        for (int i = 0; i < Size; ++i)
        {
            result.data[i] = v1.data[i] + v2.data[i];
        }

        return result;
    }

    template <int Size> inline vec<Size> operator-(const vec<Size>& v1, const vec<Size>& v2)
    {
        vec<Size> result;

        for (int i = 0; i < Size; ++i)
        {
            result.data[i] = v1.data[i] - v2.data[i];
        }

        return result;
    }

    template <int Size> inline vec<Size> operator*(const vec<Size>& v1, const vec<Size>& v2)
    {
        vec<Size> result;

        for (int i = 0; i < Size; ++i)
        {
            result.data[i] = v1.data[i] * v2.data[i];
        }

        return result;
    }

    template <int Size> inline vec<Size> operator/(const vec<Size>& v1, const vec<Size>& v2)
    {
        vec<Size> result;

        for (int i = 0; i < Size; ++i)
        {
            result.data[i] = v1.data[i] / v2.data[i];
        }

        return result;
    }

    template <int Size> inline vec<Size> operator*(const vec<Size>& v, real x)
    {
        vec<Size> result;

        for (int i = 0; i < Size; ++i)
        {
            result.data[i] = v.data[i] * x;
        }

        return result;
    }

    template <int Size> inline vec<Size> operator*(real x, const vec<Size>& v)
    {
        vec<Size> result;

        for (int i = 0; i < Size; ++i)
        {
            result.data[i] = x * v.data[i];
        }

        return result;
    }

    template <int Size> inline vec<Size> operator/(const vec<Size>& v, real x)
    {
        vec<Size> result;

        for (int i = 0; i < Size; ++i)
        {
            result.data[i] = v.data[i] / x;
        }

        return result;
    }

    template <int Size> inline vec<Size> operator/(real x, const vec<Size>& v)
    {
        vec<Size> result;

        for (int i = 0; i < Size; ++i)
        {
            result.data[i] = x / v.data[i];
        }

        return result;
    }

    template <int Size> inline real Dot(const vec<Size>& v1, const vec<Size>& v2)
    {
        real result(0);

        for (int i = 0; i < Size; ++i)
        {
            result += (v1.data[i] * v2.data[i]);
        }

        return result;
    }

    template <int Size> inline real LengthSquared(const vec<Size>& v)
    {
        real result = Dot(v, v);
        return result;
    }

    template <int Size> inline real Length(const vec<Size>& v)
    {
        real result = Sqrt(LengthSquared(v));
        return result;
    }

    template <int Size> inline real DistSquared(const vec<Size>& v1, const vec<Size>& v2)
    {
        real result = LengthSquared(v2 - v1);
        return result;
    }

    template <int Size> inline real Dist(const vec<Size>& v1, const vec<Size>& v2)
    {
        real result = Length(v2 - v1);
        return result;
    }

    template <int Size> inline vec<Size> Normalize(const vec<Size>& v)
    {
        vec<Size> result = v * InvSqrt(LengthSquared(v));
        return result;
    }

    inline real Cross(const vec<2>& v1, const vec<2>& v2)
    {
        real result = v1.x() * v2.y() - v1.y() * v2.x();
        return result;
    }

    inline vec<3> Cross(const vec<3>& v1, const vec<3>& v2)
    {
        vec<3> result = {
            v1.y() * v2.z() - v1.z() * v2.y(),
            v1.z() * v2.x() - v1.x() * v2.z(),
            v1.x() * v2.y() - v1.y() * v2.x(),
        };

        return result;
    }

    template <int Size> inline mat<Size>::mat() {}

    template <int Size> inline mat<Size>::mat(real x)
    {
        for (int i = 0; i < Size; ++i)
        {
            for (int j = 0; j < Size; ++j)
            {
                rows[i][j] = (i == j ? x : 0);
            }
        }
    }

    template <int Size> inline mat<Size>::mat(const vec<Size>& row0,
                                              const vec<Size>& row1)
    {
        static_assert(Size == 2, "mat(row0, row1) is only valid for matrices of size 2.");
        rows[0] = row0;
        rows[1] = row1;
    }

    template <int Size> inline mat<Size>::mat(const vec<Size>& row0,
                                              const vec<Size>& row1,
                                              const vec<Size>& row2)
    {
        static_assert(Size == 3, "mat(row0, row1, row2) is only valid for matrices of size 3.");
        rows[0] = row0;
        rows[1] = row1;
        rows[2] = row2;
    }

    template <int Size> inline mat<Size>::mat(const vec<Size>& row0,
                                              const vec<Size>& row1,
                                              const vec<Size>& row2,
                                              const vec<Size>& row3)
    {
        static_assert(Size == 4, "mat(row0, row1, row2, row3) is only valid for matrices of size 4.");
        rows[0] = row0;
        rows[1] = row1;
        rows[2] = row2;
        rows[3] = row3;
    }

    template <int Size> inline mat<Size>& mat<Size>::operator+=(const mat<Size>& m)
    {
        for (int i = 0; i < Size * Size; ++i)
        {
            data[i] += m.data[i];
        }

        return *this;
    }

    template <int Size> inline mat<Size>& mat<Size>::operator-=(const mat<Size>& m)
    {
        for (int i = 0; i < Size * Size; ++i)
        {
            data[i] -= m.data[i];
        }

        return *this;
    }

    template <int Size> inline mat<Size>& mat<Size>::operator*=(real x)
    {
        for (int i = 0; i < Size * Size; ++i)
        {
            data[i] *= x;
        }

        return *this;
    }

    template <int Size> inline mat<Size>& mat<Size>::operator/=(real x)
    {
        for (int i = 0; i < Size * Size; ++i)
        {
            data[i] /= x;
        }

        return *this;
    }

    template <int Size> inline mat<Size>& mat<Size>::operator*=(const mat<Size>& m)
    {
        *this = (*this) * m;
        return *this;
    }

    template <int Size> inline mat<Size>& mat<Size>::operator/=(const mat<Size>& m)
    {
        (*this) *= Inverse(m);
        return *this;
    }

    template <int Size> inline vec<Size>& mat<Size>::operator[](int index)
    {
        return rows[index];
    }

    template <int Size> inline const vec<Size>& mat<Size>::operator[](int index) const
    {
        return rows[index];
    }

    template <int Size> inline vec<Size> mat<Size>::Row(int index) const
    {
        vec<Size> result = rows[index];
        return result;
    }

    template <int Size> inline vec<Size> mat<Size>::Col(int index) const
    {
        vec<Size> result;

        for (int i = 0; i < Size; ++i)
        {
            result[i] = rows[i][index];
        }

        return result;
    }

    template <int Size> inline mat<Size> mat<Size>::Identity()
    {
        mat<Size> result(1.0);
        return result;
    }

    template <int Size> inline bool operator==(const mat<Size>& a, const mat<Size>& b)
    {
        bool result = true;

        for (int i = 0; i < Size; ++i)
        {
            if (a[i] != b[i])
            {
                result = false;
                break;
            }
        }

        return result;
    }

    template <int Size> inline mat<Size> operator-(const mat<Size>& m)
    {
        mat<Size> result;

        for (int i = 0; i < Size * Size; ++i)
        {
            result.data[i] = -m.data[i];
        }

        return result;
    }

    template <int Size> inline mat<Size> operator+(const mat<Size>& m1, const mat<Size>& m2)
    {
        mat<Size> result;

        for (int i = 0; i < Size * Size; ++i)
        {
            result.data[i] = m1.data[i] + m2.data[i];
        }

        return result;
    }

    template <int Size> inline mat<Size> operator-(const mat<Size>& m1, const mat<Size>& m2)
    {
        mat<Size> result;

        for (int i = 0; i < Size * Size; ++i)
        {
            result.data[i] = m1.data[i] - m2.data[i];
        }

        return result;
    }

    template <int Size> inline mat<Size> operator*(const mat<Size>& m, real x)
    {
        mat<Size> result;

        for (int i = 0; i < Size * Size; ++i)
        {
            result.data[i] = m.data[i] * x;
        }

        return result;
    }

    template <int Size> inline mat<Size> operator*(real x, const mat<Size>& m)
    {
        mat<Size> result;

        for (int i = 0; i < Size * Size; ++i)
        {
            result.data[i] = x * m.data[i];
        }

        return result;
    }

    template <int Size> inline vec<Size> operator*(const mat<Size>& m, const vec<Size>& v)
    {
        vec<Size> result;

        for (int i = 0; i < Size; ++i)
        {
            result[i] = Dot(m.Row(i), v);
        }

        return result;
    }

    template <int Size> inline vec<Size> operator*(const vec<Size>& v, const mat<Size>& m)
    {
        vec<Size> result;

        for (int i = 0; i < Size; ++i)
        {
            result[i] = Dot(v, m.Col(i));
        }

        return result;
    }

    template <int Size> inline mat<Size> operator*(const mat<Size>& m1, const mat<Size>& m2)
    {
        mat<Size> result;

        for (int row = 0; row < Size; ++row)
        {
            for (int col = 0; col < Size; ++col)
            {
                real prod = Dot(m1.Row(row), m2.Col(col));
                result[row][col] = prod;
            }
        }

        return result;
    }

    template <int Size> inline mat<Size> operator/(const mat<Size>& m1, const mat<Size>& m2)
    {
        mat<Size> result = m1 * Inverse(m2);
        return result;
    }

    template <int Size> inline mat<Size> operator/(const mat<Size>& m, real x)
    {
        mat<Size> result;

        for (int i = 0; i < Size; ++i)
        {
            result.data[i] = m.data[i] / x;
        }

        return result;
    }

    template <int Size> inline mat<Size> Transpose(const mat<Size>& m)
    {
        mat<Size> result;

        for (int row = 0; row < Size; ++row)
        {
            for (int col = 0; col < Size; ++col)
            {
                result[col][row] = m[row][col];
            }
        }

        return result;
    }

    template <int Size> inline real Determinant(const mat<Size>& m)
    {
        return 0.0f;
    }

    template <> inline real Determinant(const mat2& m)
    {
        real result = m[0][0] * m[1][1] - m[0][1] * m[1][0];
        return result;
    }

    template <> inline real Determinant(const mat3& m)
    {
        const real a = m[0][0] * m[1][1] * m[2][2];
        const real b = m[0][1] * m[1][2] * m[2][0];
        const real c = m[0][2] * m[1][0] * m[2][1];
        const real d = m[0][1] * m[1][0] * m[2][2];
        const real e = m[0][0] * m[1][2] * m[2][1];
        const real f = m[0][2] * m[1][1] * m[2][0];

        real result = a + b + c - d - e - f;
        return result;
    }

    template <> inline real Determinant(const mat4& m)
    {
        real a = m[0][0] * Determinant(mat3({ m[1][1], m[1][2], m[1][3] },
                                            { m[2][1], m[2][2], m[2][3] },
                                            { m[3][1], m[3][2], m[3][3] }));

        real b = m[0][1] * Determinant(mat3({ m[1][0], m[1][2], m[1][3] },
                                            { m[2][0], m[2][2], m[2][3] },
                                            { m[3][0], m[3][2], m[3][3] }));

        real c = m[0][2] * Determinant(mat3({ m[1][0], m[1][1], m[1][3] },
                                            { m[2][0], m[2][1], m[2][3] },
                                            { m[3][0], m[3][1], m[3][3] }));

        real d = m[0][3] * Determinant(mat3({ m[1][0], m[1][1], m[1][2] },
                                            { m[2][0], m[2][1], m[2][2] },
                                            { m[3][0], m[3][1], m[3][2] }));

        real result = a - b + c - d;
        return result;
    }

    template <int Size> inline mat<Size> Inverse(const mat<Size>& m)
    {
        mat<Size> matrix_of_minors;

        for (int i = 0; i < Size; ++i)
        {
            for (int j = 0; j < Size; ++j)
            {
                mat<Size-1> minor;

                for (int k = 0, row = 0; row < Size; ++row)
                {
                    if (row != i)
                    {
                        for (int l = 0, col = 0; col < Size; ++col)
                        {
                            if (col != j)
                            {
                                minor[k][l] = m[row][col];
                                ++l;
                            }
                        }
                        ++k;
                    }
                }

                matrix_of_minors[i][j] = Determinant(minor);
            }
        }

        mat<Size> cofactor_matrix = Cofactor(matrix_of_minors);
        real inv_det = real(1) / Determinant(m);

        mat<Size> result = inv_det * Transpose(cofactor_matrix);
        return result;
    }

    template <> inline mat2 Inverse(const mat2& m)
    {
        mat2 result;
        real inv_det = real(1) / Determinant(m);

        result[0][0] =  inv_det * m[1][1];
        result[0][1] = -inv_det * m[0][1];
        result[1][0] = -inv_det * m[1][0];
        result[1][1] =  inv_det * m[0][0];

        return result;
    };

    template <int Size> inline mat<Size> Cofactor(const mat<Size>& m)
    {
        mat<Size> result;

        for (int i = 0; i < Size; ++i)
        {
            for (int j = 0; j < Size; ++j)
            {
                result[i][j] = Pow(-1, i + j) * m[i][j];
            }
        }

        return result;
    }

    template <int Size> inline mat<Size> OuterProduct(const vec<Size>& u, const vec<Size>& v)
    {
        mat<Size> result;

        for (int row = 0; row < Size; ++row)
        {
            for (int col = 0; col < Size; ++col)
            {
                result[row][col] = u[row] * v[col];
            }
        }

        return result;
    }

    template <int Size> inline vec<Size-1> operator*(const mat<Size>& m, const vec<Size-1>& v)
    {
        vec<Size> v1;
        for (int i = 0; i < Size-1; ++i)
        {
            v1[i] = v[i];
        }
        v1[Size-1] = real(1);

        v1 = m * v1;
        v1 /= v1[Size - 1];

        vec<Size-1> result;
        for (int i = 0; i < Size - 1; ++i)
        {
            result[i] = v1[i];
        }

        return result;
    }

    template <int Size> inline vec<Size-1> operator*(const vec<Size-1>& v, const mat<Size>& m)
    {
        vec<Size> v1;
        for (int i = 0; i < Size-1; ++i)
        {
            v1[i] = v[i];
        }
        v1[Size-1] = real(1);

        v1 = v1 * m;
        v1 /= v1[Size - 1];

        vec<Size-1> result;
        for (int i = 0; i < Size - 1; ++i)
        {
            result[i] = v1[i];
        }

        return result;
    }

    inline mat3 CrossMatrix(const vec3& u)
    {
        mat3 result(0);

        result[0][1] = -u.z();
        result[0][2] = u.y();
        result[1][0] = u.z();
        result[1][2] = -u.x();
        result[2][0] = -u.y();
        result[2][1] = u.x();

        return result;
    }

    inline mat4 CrossMatrixHomogeneous(const vec3& u)
    {
        mat4 result(0);

        result[0][1] = -u.z();
        result[0][2] = u.y();
        result[1][0] = u.z();
        result[1][2] = -u.x();
        result[2][0] = -u.y();
        result[2][1] = u.x();
        result[3][3] = real(1);

        return result;
    }

    inline mat4 LookAt(const vec3& eye, const vec3& target, const vec3& up)
    {
        vec3 f = Normalize(target - eye);
        vec3 u = Normalize(up);
        vec3 s = Normalize(Cross(f, u));
        u = Cross(s, f);
        f = -f;

        mat4 m(1);
        m[0][0] = s.x();
        m[0][1] = s.y();
        m[0][2] = s.z();
        m[1][0] = u.x();
        m[1][1] = u.y();
        m[1][2] = u.z();
        m[2][0] = f.x();
        m[2][1] = f.y();
        m[2][2] = f.z();

        mat4 t(1);
        t[0][3] = -eye.x();
        t[1][3] = -eye.y();
        t[2][3] = -eye.z();

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
        mat3 result = Translation(t.x(), t.y());
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
        mat3 result = Scale(s.x(), s.y());
        return result;
    }

    inline mat4 Translation(real tx, real ty, real tz)
    {
        mat4 result(1);

        result[0][3] = tx;
        result[1][3] = ty;
        result[2][3] = tz;

        return result;
    }

    inline mat4 Translation(const vec3& t)
    {
        mat4 result = Translation(t.x(), t.y(), t.z());
        return result;
    }

    inline mat4 Rotation(real rx, real ry, real rz)
    {
        mat4 x(1), y(1), z(1);

        const real sx = Sin(rx);
        const real cx = Cos(rx);
        const real sy = Sin(ry);
        const real cy = Cos(ry);
        const real sz = Sin(rz);
        const real cz = Cos(rz);

        x[1][1] = cx;
        x[1][2] = -sx;
        x[2][1] = sx;
        x[2][2] = cx;

        y[0][0] = cy;
        y[0][2] = -sy;
        y[2][0] = sy;
        y[2][2] = cy;

        z[0][0] = cz;
        z[0][1] = -sz;
        z[1][0] = sz;
        z[1][1] = cz;

        // TODO(Charly): Check multiplication order
        mat4 result = x * y * z;
        return result;
    }

    inline mat4 Rotation(const vec3& r)
    {
        mat4 result = Rotation(r.x(), r.y(), r.z());
        return result;
    }

    inline mat4 Rotation(const vec3& axis, real angle)
    {
        mat4 result;

        const real ca = Cos(angle);
        const real sa = Sin(angle);
        const real ica = 1 - ca;

        const real x = axis[0];
        const real y = axis[1];
        const real z = axis[2];

        const real xx = x * x;
        const real xy = x * y;
        const real xz = x * z;
        const real yy = y * y;
        const real yz = y * z;
        const real zz = z * z;

        result[0][0] = ca + xx * ica;
        result[0][1] = xy * ica - z * sa;
        result[0][2] = xz * ica + y * sa;

        result[1][0] = xy * ica + z * sa;
        result[1][1] = ca + yy * ica;
        result[1][2] = yz * ica - x * sa;

        result[2][0] = xz * ica - y * sa;
        result[2][1] = yz * ica + x * sa;
        result[2][2] = ca + zz * ica;

        result[3][3] = real(1);

        return result;
    }

    inline mat4 Scale(real sx, real sy, real sz)
    {
        mat4 result(1);
        result[0][0] = sx;
        result[1][1] = sy;
        result[2][2] = sz;

        return result;
    }

    inline mat4 Scale(const vec3& s)
    {
        mat4 result = Scale(s.x(), s.y(), s.z());
        return result;
    }
}

#endif // ZMATH_HPP
