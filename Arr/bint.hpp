//
//  bint.hpp
//  Arr
//
//  Created by DB on 11/21/18.
//  Copyright © 2018 D Blalock. All rights reserved.
//

#ifndef bint_h
#define bint_h

// #include <type_traits>

// #include "macros.hpp"

#define MIN(X, Y) ( ((X) <= (Y)) ? (X) : (Y) )
#define MAX(X, Y) ( ((X) >= (Y)) ? (X) : (Y) )

// template<int Min, int Max, class IntT=int32_t>
template<int Min, int Max=Min>
struct bint {
    static const int min = Min;
    static const int max = Max;
    static const bool is_valid = min <= max;
    static const bool is_nonneg = (max >= 0) && (min >= 0);
    static const bool is_pos = (max > 0) && (min > 0);

    // TODO determine number of bits needed
    // static const int needed_bits_signed = MAX(NEEDED_NBITS_SIGNED(min))


    // IntT value;
};

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



template<int min1, int max1, int min2, int max2>
bint<min1 + min2, max1 + max2>
operator+(const bint<min1, max1>& lhs, const bint<min2, max2>& rhs) {
    return bint<min1 + min2, max1 + max2>();
}


#endif /* bint_h */
