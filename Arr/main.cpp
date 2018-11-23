//
//  main.cpp
//  Arr
//
//  Created by DB on 11/21/18.
//  Copyright © 2018 D Blalock. All rights reserved.
//

#include <iostream>

#include <assert.h>

#include "arrayview.hpp"
#include "bint.hpp"

#include "array_utils.hpp"

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";


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
    
    const int n = 12;
    float vals[n];
    for (int i = 0; i < n; i++) { vals[i] = i; }
    ArrayView<float> v(&vals[0], {n});
    ArrayView<float, AxesRowMajor2D> v2(&vals[0], {4,3});
    
//    printf("%g\n", v[7]);
    printf("elem 2, 3: %g\n", v2[{(int64_t)2, (int64_t)3}]);
    
    return 0;
}
