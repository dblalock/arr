//
//  bint.hpp
//  Arr
//
//  Created by DB on 11/21/18.
//  Copyright © 2018 D Blalock. All rights reserved.
//

#ifndef bint_h
#define bint_h

#include "macros.hpp"  // for REQUIRE_INT, REQUIRE_NUM

// #include <type_traits>

// #include "macros.hpp"

#define MIN(X, Y) ( ((X) <= (Y)) ? (X) : (Y) )
#define MAX(X, Y) ( ((X) >= (Y)) ? (X) : (Y) )

// TODO implement compile-time add, sub, mul, div using TMP

// using bint_int_t = int32_t;

// template<int Max=-1, int Min=0, bool Valid=true, class IntT=int32_t>
template<int Max=-1, int Min=0, bool Valid=true>
class bint {
public:
    static const int min = MIN(Min, Max);
    static const int max = MAX(Min, Max);
    // static const bool is_valid = min <= max;
    static const bool is_nonneg = (max >= 0) && (min >= 0);
    static const bool is_pos = (max > 0) && (min > 0);
    static const bool is_bint = true; // to ease checks for non-bint types
    using int_t = int;

    // TODO we could could extend operator overloads to prove no
    // int overflows; set valid to false if one is possible
    static const bool is_valid = Valid;

    // TODO determine number of bits needed
    // static const int needed_bits_signed = MAX(NEEDED_NBITS_SIGNED(min))

    bint(int_t value): val(value) {};
    int_t get() const { return val; }

    template<class T> T cast() const { return static_cast<T>(val); }

private:
    int_t val;
};

// using NoBounds = bint<0, 0, false>;


//template<class T, bool Valid=T::is_valid> struct is_valid { static const bool value = Valid; };
//template<class T> struct is_valid { static const bool value = false; };

// template<class T>
// template<class T, int max=0, int min=0>
// template<class T, int Max=0, int Min=0, bool Valid=true, class IntT=int32_t>
// struct is_bint : std::false_type {};
// template<> struct is_bint<bint<Max, Min, Valid, IntT> > : std::true_type {};


template<int min1, int max1, int min2, int max2, bool valid1, bool valid2>
bint<min1 + min2, max1 + max2, valid1 && valid2>
operator+(const bint<min1, max1, valid1>& lhs,
    const bint<min2, max2, valid2>& rhs)
{
    return bint<min1 + min2, max1 + max2, valid1 && valid2>(lhs.get() + rhs.get());
}

template<int min1, int max1, int min2, int max2, bool valid1, bool valid2>
bint<MIN(min1 - min2, min1 - max2), MAX(max1 - min1, max1 - max2), valid1 && valid2>
operator-(const bint<min1, max1, valid1>& lhs,
    const bint<min2, max2, valid2>& rhs)
{
    return bint<MIN(min1 - min2, min1 - max2), MAX(max1 - min1, max1 - max2), valid1 && valid2>(
        lhs.get() - rhs.get());
}


// ------------------------ plus scalar
template<int min1, int max1, class T, REQUIRE_NUM(T)>
auto operator+(const bint<min1, max1>& bint, const T& other)
    -> decltype(bint.get() + other)
{
    return bint.get() + other;
}
template<int min1, int max1, class T, REQUIRE_NUM(T)>
auto operator+(const T& other, const bint<min1, max1>& bint)
    -> decltype(other + bint.get())
{
    return other + bint.get();
}
// ------------------------ minus scalar
template<int min1, int max1, class T, REQUIRE_NUM(T)>
auto operator-(const bint<min1, max1>& bint, const T& other)
    -> decltype(bint.get() - other)
{
    return bint.get() - other;
}
template<int min1, int max1, class T, REQUIRE_NUM(T)>
auto operator-(const T& other, const bint<min1, max1>& bint)
    -> decltype(other - bint.get())
{
    return other - bint.get();
}
// ------------------------ times scalar
template<int min1, int max1, class T, REQUIRE_NUM(T)>
auto operator*(const bint<min1, max1>& bint, const T& other)
    -> decltype(bint.get() * other)
{
    return bint.get() * other;
}
template<int min1, int max1, class T, REQUIRE_NUM(T)>
auto operator*(const T& other, const bint<min1, max1>& bint)
    -> decltype(other * bint.get())
{
    return other * bint.get();
}
// ------------------------ div scalar
template<int min1, int max1, class T, REQUIRE_NUM(T)>
auto operator/(const bint<min1, max1>& bint, const T& other)
    -> decltype(bint.get() / other)
{
    return bint.get() / other;
}
template<int min1, int max1, class T, REQUIRE_NUM(T)>
auto operator/(const T& other, const bint<min1, max1>& bint)
    -> decltype(other / bint.get())
{
    return other / bint.get();
}


// only defining this for nonnegative X, Y
#define X_DIVIDES_Y(X, Y) (((X) > 0) && ((Y) > 0) && (((Y) % (X)) == 0) )
#define COMBINED_STRIDE(X, Y) \
    (X_DIVIDES_Y(X, Y) || X_DIVIDES_Y(Y, X)) ? MIN(X, Y) : 0


// template<int min, int max>
// struct is_valid<bint<min, max> > { static const int val = max >= min; };
// template<int min, int max>
// struct is_const<bint<min, max> > { static const int val = min == max; };

// template<class T>
// struct is_nonneg {
//     static const int value = (T::max >= 0) && (T::min >= 0);
// };

// template<int min, int max>
// struct is_pos<bint<min, max> > {
//     static const int val = (max > 0) && (min > 0);
// };



#endif /* bint_h */
