//
//  main.cpp
//  Arr
//
//  Created by DB on 11/21/18.
//  Copyright © 2018 D Blalock. All rights reserved.
//

#include <iostream>

#include <assert.h>

#define USE_ARRAYVIEW_VERSION 2

#if USE_ARRAYVIEW_VERSION == 1
    #include "arrayview.hpp"
    #include "bint.hpp"
#elif USE_ARRAYVIEW_VERSION == 2
    #include "arrayview2.hpp"
#endif

#include "array_utils.hpp"

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";

    using namespace ar;

#if USE_ARRAYVIEW_VERSION == 1
    bint<5, 0> i(3);
    bint<1, 3> j(1);
    auto c = i + j;
    auto d = c + 7.5;
    printf("d = %g\n", d);

    static_assert(decltype(c)::is_nonneg, "c not nonnegative!");

    // printf("5 %% 0 = %d\n", 5 % 0);
    printf("5 %% -2 = %d\n", 5 % -2);
    printf("0 %% 5 = %d\n", 0 % 5);
    // printf("0 %% 0 = %d\n", 0 % 0);
    printf("0 %% -2 = %d\n", 0 % -2);
    printf("-2 %% 7 = %d\n", -2 % 7);
    printf("-2 %% -5 = %d\n", -2 % -5);
    printf("7 %% -2 = %d\n", 7 % -2);
    printf("-5 %% -2 = %d\n", -5 % -2);

    auto strides1D = default_strides_for_shape<AxesDense1D>({10});
//    printf("%d\n", strides1D[0]);
    assert(strides1D[0] == 1);
    assert(strides1D.size() == 1);
    
    const int n = 24;
    float vals[n];
    for (int i = 0; i < n; i++) { vals[i] = i; }
    ArrayView<float> v(&vals[0], {n});
    ArrayView<float, AxesRowMajor2D> v2(&vals[0], {4,3});
#elif USE_ARRAYVIEW_VERSION == 2
    const int n = 24;
    float vals[n];
    for (int i = 0; i < n; i++) { vals[i] = i; }
    ArrayView<float> v(&vals[0], {n});
    ArrayView<float, ar::format::RowMajor2D> v2(&vals[0], {4,3});
    
#endif
    auto ar2d = make_view(vals, 2, 12);
    auto ar3d = make_view(vals, 2, 6, 2);
    auto ar4d = make_view(vals, 2, 2, 2, 3);
    
//    printf("%g\n", v[7]);
    printf("ar v2: elem 3, 2: %g\n", v2[{3, 2}]);
    printf("elem 1, 11: %g\n", ar2d[{1, 11}]);
    printf("elem 1, 5, 1: %g\n", ar3d[{1, 5, 1}]);
    printf("elem 1, 5, 1: %g\n", ar4d[{1, 1, 1, 2}]);
    
    return 0;
}
